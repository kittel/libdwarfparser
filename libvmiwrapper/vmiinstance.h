#ifndef _VMIINSTANCE_H_
#define _VMIINSTANCE_H_

#undef _GNU_SOURCE
#include <libvmi/libvmi.h>
#include <libvmi/libvmi_extra.h>
#include <libvmi/x86.h>

#include <string>

#include <map>


typedef std::map<uint64_t, page_info_t*> PageMap;
		

class VMIInstance {

	private:
		vmi_instance_t vmi;
		static VMIInstance *instance;
		void pauseVM();
		void resumeVM();

	public:
		VMIInstance(std::string name, uint32_t flags = 0);
		virtual ~VMIInstance();

		static VMIInstance *getInstance();
		vmi_instance_t getLibVMIInstance();

		uint8_t read8FromVA(uint64_t va, uint32_t pid = 0);
		uint16_t read16FromVA(uint64_t va, uint32_t pid = 0);
		uint32_t read32FromVA(uint64_t va, uint32_t pid = 0);
		uint64_t read64FromVA(uint64_t va, uint32_t pid = 0);
		std::string readStrFromVA(uint64_t va, uint32_t pid = 0);
		void printKernelPages();

		PageMap destroyMap(PageMap map);
		PageMap getExecutableKernelPages();
		PageMap getKernelPages();

};

#endif /* _VMIINSTANCE_H_ */
