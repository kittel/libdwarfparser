#ifndef _VMIINSTANCE_H_
#define _VMIINSTANCE_H_

#undef _GNU_SOURCE
#include <libvmi/libvmi.h>
#include <libvmi/libvmi_extra.h>
#include <libvmi/x86.h>

#include <string>


class VMIInstance {

	private:
		vmi_instance_t vmi;
		static VMIInstance *instance;
		void pauseVM();
		void resumeVM();
		void getKernelPages();

	public:
		VMIInstance(std::string name, uint32_t flags = 0);
		virtual ~VMIInstance();

		static VMIInstance *getInstance();
		vmi_instance_t getLibVMIInstance();

		uint8_t read8FromVA(uint64_t va, uint32_t pid = 0);
		uint16_t read16FromVA(uint64_t va, uint32_t pid = 0);
		uint32_t read32FromVA(uint64_t va, uint32_t pid = 0);
		uint64_t read64FromVA(uint64_t va, uint32_t pid = 0);
		GSList *get_Kernel_Pages(vmi_instance_t *vmi);

};

#endif /* _VMIINSTANCE_H_ */
