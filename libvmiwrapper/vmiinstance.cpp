#include "vmiinstance.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "vmiexception.h"

#include "helpers.h"

#define IA32E_IS_PAGE_USER_SUPERVISOR(page_info)        \
	(USER_SUPERVISOR(page_info->x86_ia32e.pte_value) && \
	 USER_SUPERVISOR(page_info->x86_ia32e.pgd_value) && \
	 USER_SUPERVISOR(page_info->x86_ia32e.pdpte_value) && \
	 USER_SUPERVISOR(page_info->x86_ia32e.pml4e_value))

#define IA32E_IS_PAGE_SUPERVISOR(page_info) (!(IA32E_IS_PAGE_USER_SUPERVISOR(page_info)))

#define PAGE_NX(entry)	        VMI_GET_BIT(entry, 63)
#define IS_PAGE_NX(page_info)                   \
	(PAGE_NX(page_info->x86_ia32e.pte_value) || \
	 PAGE_NX(page_info->x86_ia32e.pgd_value) || \
	 PAGE_NX(page_info->x86_ia32e.pdpte_value) || \
	 PAGE_NX(page_info->x86_ia32e.pml4e_value))

/* GLIB Macros */

#define GFOREACH(item, list) for(GList *__glist = list; __glist && (item = __glist->data, true); __glist = __glist->next)

VMIInstance *VMIInstance::instance = NULL;

VMIInstance::VMIInstance(std::string name, vmi_mode flags)
	:
	vmiMutex(),
	paused(false) {

	if (!flags) {
		flags = VMI_XEN;
	}

	/* initialize the libvmi library */
	//char *name_i = const_cast<char *>(name.c_str());
	if (vmi_init(&vmi, flags, (void*)name.c_str(),VMI_INIT_DOMAINNAME,NULL,NULL) == VMI_FAILURE) {
		printf("Failed to init LibVMI library.\n");
		throw VMIException();
	}

	/* init the offset values */
	if (VMI_OS_LINUX != vmi_get_ostype(vmi)) {
		printf("This tool only supports Linux\n");
		throw VMIException();
	}

	instance = this;
	return;
}

VMIInstance::~VMIInstance() {
	/* resume the vm */
	this->resumeVM();

	instance = NULL;

	/* cleanup any memory associated with the LibVMI instance */
	vmi_destroy(vmi);
	return;
}

VMIInstance *VMIInstance::getInstance() {
	return VMIInstance::instance;
}

void VMIInstance::pauseVM() {
	/* pause the vm for consistent memory access */
	if (vmi_pause_vm(vmi) != VMI_SUCCESS) {
		printf("Failed to pause VM\n");
		throw VMIException();
	}
	paused = true;
}

void VMIInstance::resumeVM() {
	/* pause the vm for consistent memory access */
	if (!paused)
		return;
	if (vmi_resume_vm(vmi) != VMI_SUCCESS) {
		printf("Failed to resume VM\n");
		throw VMIException();
	}
	paused = false;
}

vmi_instance_t VMIInstance::getLibVMIInstance() {
	return vmi;
}

void VMIInstance::printKernelPages() const {
	addr_t init_dtb = vmi_pid_to_dtb(vmi, 1);

	printf("Init dtb is at %p\n", (void *)init_dtb);

	GSList *pages = vmi_get_va_pages(vmi, init_dtb);

	if (pages != NULL)
		printf("Got Pages\n");

	page_info_t *item;

	for (GSList *__glist = pages; __glist; __glist = __glist->next) {
		item = (page_info_t *)__glist->data;
		if (item->x86_ia32e.pml4e_value == 0) {
			printf("l4_v is null\n");
		} else {
			// if(ENTRY_PRESENT(item->x86_ia32e.pte_value, VMI_OS_LINUX) &&
			// IA32E_IS_PAGE_SUPERVISOR(item)){
			if (ENTRY_PRESENT(item->x86_ia32e.pte_value, VMI_OS_LINUX) &&
			    item->vaddr >> 40 != 0x88) {
				if (IA32E_IS_PAGE_USER_SUPERVISOR(item))
					continue;
				printf("Page at %16lx : %016lx - %s, %s ",
				       item->vaddr,
				       item->paddr,
				       (IA32E_IS_PAGE_SUPERVISOR(item)) ? "S" : "U",
				       (IS_PAGE_NX(item)) ? "NX" : "X");
				printf("Page size: %i\n", item->size);

				// printf("Settings: %i - %i - %i - %i\n",
				//    USER_SUPERVISOR(item->x86_ia32e.pte_value),
				//    USER_SUPERVISOR(item->x86_ia32e.pgd_value),
				//    USER_SUPERVISOR(item->x86_ia32e.pdpte_value),
				//    USER_SUPERVISOR(item->x86_ia32e.pml4e_value));
			}
		}
		free(item);
	}
	free(pages);
}

PageMap VMIInstance::destroyMap(PageMap map) const {
	for (auto &item : map) {
		free(item.second);
	}
	map.clear();
	return map;
}

PageMap VMIInstance::getPages(uint32_t pid, bool executable) {
	vmiMutex.lock();
	addr_t init_dtb = vmi_pid_to_dtb(vmi, ((pid == 0) ? 1 : pid));
	GSList *pages = vmi_get_va_pages(vmi, init_dtb);
	vmiMutex.unlock();
	assert(pages);

	page_info_t *item;
	PageMap pageMap;

	for (GSList *__glist = pages; __glist; __glist = __glist->next) {
		item = (page_info_t *)__glist->data;
		if (item->x86_ia32e.pml4e_value != 0 &&
		    ENTRY_PRESENT(item->x86_ia32e.pte_value, VMI_OS_LINUX) &&
		    ((pid == 0 && IA32E_IS_PAGE_SUPERVISOR(item)) ||
		     (pid != 0 && !IA32E_IS_PAGE_SUPERVISOR(item))) &&
		    (!executable || !IS_PAGE_NX(item))) {
			pageMap.insert(
			    std::pair<uint64_t, page_info_t *>(item->vaddr, item));
		} else {
			free(item);
		}
	}
	g_slist_free(pages);
	return pageMap;
}

uint64_t VMIInstance::translateV2P(uint64_t va, uint32_t pid) {
	std::lock_guard<std::mutex> lock(vmiMutex);
	if (pid) {
		return vmi_translate_uv2p(vmi, va, pid);
	}
	return vmi_translate_kv2p(vmi, va);
}

uint8_t VMIInstance::read8FromVA(uint64_t va, uint32_t pid) {
	uint8_t value = 0;
	vmiMutex.lock();
	vmi_read_8_va(vmi, va, pid, &value);
	vmiMutex.unlock();
	return value;
}

uint16_t VMIInstance::read16FromVA(uint64_t va, uint32_t pid) {
	uint16_t value = 0;
	vmiMutex.lock();
	vmi_read_16_va(vmi, va, pid, &value);
	vmiMutex.unlock();
	return value;
}

uint32_t VMIInstance::read32FromVA(uint64_t va, uint32_t pid) {
	uint32_t value = 0;
	vmiMutex.lock();
	vmi_read_32_va(vmi, va, pid, &value);
	vmiMutex.unlock();
	return value;
}

uint64_t VMIInstance::read64FromVA(uint64_t va, uint32_t pid) {
	uint64_t value = 0;
	vmiMutex.lock();
	vmi_read_64_va(vmi, va, pid, &value);
	vmiMutex.unlock();
	return value;
}

std::vector<uint8_t> VMIInstance::readVectorFromVA(uint64_t va,
                                                   uint64_t len,
                                                   uint32_t pid,
                                                   bool fillunmapped) {
	uint8_t *buffer = (uint8_t *)malloc(len);
	memset(buffer, 0, len);
	std::vector<uint8_t> result;
	size_t size = 0;
	size_t res  = 0;

	while (size < len) {
		vmiMutex.lock();
		res = vmi_read_va(vmi,
		                  va + size,
		                  pid,
		                  buffer + size,
		                  std::min(0x1000, (int)(len - size)));
		vmiMutex.unlock();
		size += res;
		if (res == 0) {
			if (fillunmapped) {
				// Try to read next page
				size += 0x1000;
			} else {
				// Unable to return more, probably not mapped
				break;
			}
		}
	}

	result.insert(result.end(), buffer, buffer + size);

	free(buffer);
	return result;
}

std::string VMIInstance::readStrFromVA(uint64_t va, uint32_t pid) {
	vmiMutex.lock();
	char *str = vmi_read_str_va(this->vmi, va, pid);
	vmiMutex.unlock();

	assert(str);
	std::string result = std::string{str};
	free(str);
	return result;
}

std::vector<uint8_t> VMIInstance::readVectorFromPA(uint64_t pa,
                                                   uint64_t len) {
	uint8_t *buffer = (uint8_t *)malloc(len);
	memset(buffer, 0, len);
	std::vector<uint8_t> result;
	size_t res  = 0;

	vmiMutex.lock();
	res = vmi_read_pa(vmi, pa, buffer, len);
	vmiMutex.unlock();

	result.insert(result.end(), buffer, buffer + res);

	free(buffer);
	return result;
}

bool VMIInstance::isPageSuperuser(page_info_t *page) const {
	bool ret = IA32E_IS_PAGE_SUPERVISOR(page);
	return ret;
}

bool VMIInstance::isPageExecutable(page_info_t *page) const {
	bool ret = !IS_PAGE_NX(page);
	return ret;
}

void VMIInstance::dumpMemory(uint64_t address,
                             uint64_t len,
                             std::string filename) {
	std::vector<uint8_t> dump = this->readVectorFromVA(address, len);

	std::ofstream file(filename, std::ios::binary);
	if (file.is_open()) {
		file.write((char *)dump.data(), dump.size());
		file.close();
	}
	return;
}
