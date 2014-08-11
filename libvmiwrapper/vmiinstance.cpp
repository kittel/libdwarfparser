#include "vmiinstance.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <inttypes.h>
#include "vmiexception.h"
#include "vmiinstance.h"

#include <libvmi/x86.h>

#include "helpers.h"

#define IS_PAGE_USER_SUPERVISOR(page_info)   (USER_SUPERVISOR(page_info->l1_v) || \
											 USER_SUPERVISOR(page_info->l2_v) || \
											 USER_SUPERVISOR(page_info->l3_v) || \
											 USER_SUPERVISOR(page_info->l4_v))

#define IS_PAGE_SUPERVISOR(page_info)        (!(IS_PAGE_USER_SUPERVISOR(page_info)))

#define PAGE_NX(entry)	        VMI_GET_BIT(entry, 63)
#define IS_PAGE_NX(page_info)				(PAGE_NX(page_info->l1_v) || \
											 PAGE_NX(page_info->l2_v) || \
											 PAGE_NX(page_info->l3_v) || \
											 PAGE_NX(page_info->l4_v))

/* GLIB Macros */

#define GFOREACH(item, list) for(GList *__glist = list; __glist && (item = __glist->data, true); __glist = __glist->next)

VMIInstance *VMIInstance::instance = NULL;

VMIInstance::VMIInstance(std::string name, uint32_t flags){
	
	if(!flags){
		flags = VMI_AUTO | VMI_INIT_COMPLETE;
	}

    /* initialize the libvmi library */
	char * name_i = const_cast<char*>(name.c_str());
    if (vmi_init(&vmi, flags, name_i) == VMI_FAILURE) {
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

VMIInstance::~VMIInstance(){
    /* resume the vm */
    this->resumeVM();

	instance = NULL;

    /* cleanup any memory associated with the LibVMI instance */
    vmi_destroy(vmi);
	return;
}

VMIInstance *VMIInstance::getInstance(){
	return VMIInstance::instance;
}

void VMIInstance::pauseVM(){
    /* pause the vm for consistent memory access */
    if (vmi_pause_vm(vmi) != VMI_SUCCESS) {
        printf("Failed to pause VM\n");
        throw VMIException();
    }
}

void VMIInstance::resumeVM(){
    /* pause the vm for consistent memory access */
    if (vmi_resume_vm(vmi) != VMI_SUCCESS) {
        printf("Failed to resume VM\n");
        throw VMIException();
    }
}

vmi_instance_t VMIInstance::getLibVMIInstance(){
	return vmi;
}

void VMIInstance::getKernelPages(){

    addr_t init_dtb = vmi_pid_to_dtb(vmi, 1);

    printf("Init dtb is at %p\n", (void*) init_dtb);

    GSList* pages = vmi_get_va_pages(vmi, init_dtb);

    if(pages != NULL) printf("Got Pages\n");

    page_info_t *item;

    for(GSList *__glist = pages; __glist ; __glist = __glist->next){
	    item = (page_info_t*) __glist->data;
	    if (item-> l4_v == 0){
            printf("l4_v is null\n");
    	}else {
			if(ENTRY_PRESENT(item->l1_v, VMI_OS_LINUX) && IS_PAGE_SUPERVISOR(item)){
				printf("Page size: %i\n", item->size);
		        printf("Page at %016lx - %i, %i\n", item->paddr, IS_PAGE_SUPERVISOR(item), IS_PAGE_NX(item));
				printf("\n");
			}
        }


    }

}

uint8_t VMIInstance::read8FromVA(uint64_t va, uint32_t pid){
	uint8_t value = 0;
	vmi_read_8_va(vmi, va, pid, &value);
	return value;
}

uint16_t VMIInstance::read16FromVA(uint64_t va, uint32_t pid){
	uint16_t value = 0;
	vmi_read_16_va(vmi, va, pid, &value);
	return value;
}

uint32_t VMIInstance::read32FromVA(uint64_t va, uint32_t pid){
	uint32_t value = 0;
	vmi_read_32_va(vmi, va, pid, &value);
	return value;
}

uint64_t VMIInstance::read64FromVA(uint64_t va, uint32_t pid){
	uint64_t value = 0;
	vmi_read_64_va(vmi, va, pid, &value);
	return value;
}

