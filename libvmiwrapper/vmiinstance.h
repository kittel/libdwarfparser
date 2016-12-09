#ifndef _VMIINSTANCE_H_
#define _VMIINSTANCE_H_

#include <string>
#include <vector>

#include <map>

#include <mutex>

#undef _GNU_SOURCE
#include <libvmi/libvmi.h>
#include <libvmi/libvmi_extra.h>
#include <libvmi/x86.h>

typedef std::map<uint64_t, page_info_t *> PageMap;

class VMIInstance {
private:
	vmi_instance_t vmi;
	static VMIInstance *instance;

	std::mutex vmiMutex;

	bool paused;
	void pauseVM();
	void resumeVM();

public:
	VMIInstance(std::string name, uint32_t flags = 0);
	virtual ~VMIInstance();

	static VMIInstance *getInstance();
	vmi_instance_t getLibVMIInstance();

	uint64_t translateV2P(uint64_t va, uint32_t pid = 0);

	uint8_t read8FromVA(uint64_t va, uint32_t pid = 0);
	uint16_t read16FromVA(uint64_t va, uint32_t pid = 0);
	uint32_t read32FromVA(uint64_t va, uint32_t pid = 0);
	uint64_t read64FromVA(uint64_t va, uint32_t pid = 0);
	std::vector<uint8_t> readVectorFromVA(uint64_t va,
	                                      uint64_t len,
	                                      uint32_t pid = 0,
	                                      bool fillunmapped = 0);
	std::string readStrFromVA(uint64_t va, uint32_t pid = 0);
	std::vector<uint8_t> readVectorFromPA(uint64_t pa,
	                                      uint64_t len);
	void printKernelPages() const;

	bool isPageSuperuser(page_info_t* page) const;
	bool isPageExecutable(page_info_t* page) const;

	PageMap destroyMap(PageMap map) const;

	PageMap getPages(uint32_t pid=0, bool executable=false);

	void dumpMemory(uint64_t address, uint64_t len, std::string filename);
};

#endif /* _VMIINSTANCE_H_ */
