/*
 * gipLicenseManager.cpp
 *
 *  Created on: Mar 11, 2023
 *      Author: Noyan Culum
 */

#include "gipLicenseManager.h"
#include "gAES.h"

std::string gipLicenseManager::foldername = "User\\Profile";


gipLicenseManager::gipLicenseManager() {

	licensename = {
			"LICENSE_INVALID", "LICENSE_TRIAL_OFFLINE", "LICENSE_FREE_OFFLINE",
			"LICENSE_STUDENT_1YEAR", "LICENSE_OEM_1YEAR", "LICENSE_ACADEMIC_4YEAR",
			"LICENSE_PERSONAL_SUBSCRIPTION", "LICENSE_BUSINESS_SUBSCRIPTION", "LICENSE_GOVERNMENTAL_SUBSCRIPTION", "LICENSE_OEM_SUBSCRIPTION",
			"LICENSE_PERSONAL_INFINITE", "LICENSE_BUSINESS_INFINITE", "LICENSE_GOVERNMENTAL_INFINITE", "LICENSE_OEM_INFINITE",
			"LICENSE_INDUSTRIAL_INFINITE_OFFLINE"
	};

	licensechecktype = {
			CHECKTYPE_NONE, CHECKTYPE_OFFLINE, CHECKTYPE_OFFLINE,
			CHECKTYPE_ONLINE, CHECKTYPE_ONLINE, CHECKTYPE_ONLINE,
			CHECKTYPE_ONLINE, CHECKTYPE_ONLINE, CHECKTYPE_ONLINE, CHECKTYPE_ONLINE,
			CHECKTYPE_ONLINE, CHECKTYPE_ONLINE, CHECKTYPE_ONLINE, CHECKTYPE_ONLINE,
			CHECKTYPE_OFFLINE
	};

	licensedurationtype = {
			DURATIONTYPE_NONE, DURATIONTYPE_LIMITED, DURATIONTYPE_INFINITE,
			DURATIONTYPE_LIMITED, DURATIONTYPE_LIMITED, DURATIONTYPE_LIMITED,
			DURATIONTYPE_LIMITED, DURATIONTYPE_LIMITED, DURATIONTYPE_LIMITED, DURATIONTYPE_LIMITED,
			DURATIONTYPE_INFINITE, DURATIONTYPE_INFINITE, DURATIONTYPE_INFINITE, DURATIONTYPE_INFINITE,
			DURATIONTYPE_INFINITE
	};

	licenserenewaltype = {
			RENEWALTYPE_NONE, RENEWALTYPE_NONE, RENEWALTYPE_FULL,
			RENEWALTYPE_NONE, RENEWALTYPE_NONE, RENEWALTYPE_NONE,
			RENEWALTYPE_SUBSCRIPTION, RENEWALTYPE_SUBSCRIPTION, RENEWALTYPE_SUBSCRIPTION, RENEWALTYPE_SUBSCRIPTION,
			RENEWALTYPE_FULL, RENEWALTYPE_FULL, RENEWALTYPE_FULL, RENEWALTYPE_FULL,
			RENEWALTYPE_FULL
	};

	licenseusagetype = {
			USAGETYPE_NONE, USAGETYPE_TRIAL, USAGETYPE_ACTUAL,
			USAGETYPE_ACTUAL, USAGETYPE_ACTUAL, USAGETYPE_ACTUAL,
			USAGETYPE_ACTUAL, USAGETYPE_ACTUAL, USAGETYPE_ACTUAL, USAGETYPE_ACTUAL,
			USAGETYPE_ACTUAL, USAGETYPE_ACTUAL, USAGETYPE_ACTUAL, USAGETYPE_ACTUAL,
			USAGETYPE_ACTUAL
	};

	chars = "23456789ABCDEFGHJKLMNPQRSTUVWXYZ"; //32 chars
	controlchars = "23456789abcdefghjklmnpqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ";
	charnum = chars.length();
	orderingschemenum = charnum;
	appguid = generateAppUID();

	developercompanyname = "Glist_Developer";
	softwarename = "glapp";
	glistproductid = softwarename;
	glistcompileid = "000000";
	glistcompileidabbr = "01x12";
	fullpath = "SOFTWARE\\" + developercompanyname + "\\" + softwarename + "\\" + foldername;
	glistenginepath = "SOFTWARE\\Openglsx1\\Apps\\" + softwarename;
	glistenginepath2 = "SOFTWARE\\Openglsx1\\Engine\\Compile\\";
	registryhive = REGISTRYHIVE_LOCALMACHINE;
	registryhivekey = HKEY_LOCAL_MACHINE;

	license = "";
	isinfinite = false;
	fulllength = letternum + (letternum / stepnum) - 1;

	olno = 6;
	alno = 10;
	wlno = 14;

	/**
	 * In the first run, the developer should comment-out these 2 lines and generate
	 * his/her own schemes. The generators will print out the schemes into the console.
	 * Copy the schemes from the console and paste them into the getOrderingScheme()
	 * and getAdditionScheme() functions, instead of old values. Then comment these
	 * 2 lines back.
	 */
//	printoutOrderingScheme();
//	printoutAdditionScheme();

	osno = 0;
	asno = 0;
	wsno = 0;

	maxtrialdate = "691231";
	licenseenddate = getDate(7);
	licensetype = LICENSE_TRIAL_OFFLINE;
	secretword = "GlistEngine";
	secretword2 = "Development";
	wordlist = {licenseenddate, gToStr(licensetype), secretword, secretword};
	generateWordScheme();

	getOrderingScheme();
	getAdditionScheme();

	md5startno = 7;
}

gipLicenseManager::~gipLicenseManager() {
}

bool gipLicenseManager::initialize(std::string companyName, std::string softwareName, std::string glistProductId, std::string glistCompileId, std::string secretKey, std::string secretKey2, std::string subFolder, int registryHive) {
	if(!isInitializersOK(companyName, softwareName, glistProductId, glistCompileId)) return false;
	developercompanyname = companyName;
	softwarename = softwareName;
	foldername = subFolder;
	fullpath = "SOFTWARE\\" + developercompanyname + "\\" + softwarename + "\\" + foldername;
	if(glistProductId == "") glistProductId = softwarename;
	glistproductid = glistProductId;
	glistenginepath = "SOFTWARE\\Openglsx1\\Apps\\" + glistproductid;
	glistcompileid = glistCompileId;

	registryhive = registryHive;
	if(registryhive == REGISTRYHIVE_LOCALMACHINE) registryhivekey = HKEY_LOCAL_MACHINE;
	else {
		registryhive = REGISTRYHIVE_CURRENTUSER;
		registryhivekey = HKEY_CURRENT_USER;
	}
	setSecretWord(secretKey, secretKey2);
//	setWindowsGUID(gWindows::getMachineGUID());
	generateWordScheme();
	return true;
}

bool gipLicenseManager::isInitializersOK(std::string companyName, std::string softwareName, std::string glistProductId, std::string glistCompileId) {
	// Check companyName
	if(companyName == "" || companyName == "GlistEngine" || companyName == "GlistApp") {
		gLoge("LM") << "Company name is not valid.";
		return false;
	}

	// Check softwareName
	if(softwareName == "" || softwareName == "GlistEngine" || softwareName == "GlistApp") {
		gLoge("LM") << "Software name is not valid.";
		return false;
	}

	// Check glistProductId
	if(glistProductId == "" || glistProductId == "GlistEngine" || glistProductId == "GlistApp") {
		gLoge("LM") << "Glist Product ID is not valid.";
		return false;
	}

	// Check glistCompileId
	int gcid = gToInt(glistCompileId);
	if(gcid < 100000 || gcid > 999999) {
		gLoge("LM") << "glistCompileId should be a app-specific number between 100000-999999";
		return false;
	}

	// Checks are OK
	return true;
}

std::string gipLicenseManager::getDeveloperCompanyName() {
	return developercompanyname;
}

std::string gipLicenseManager::getSoftwareName() {
	return softwarename;
}

std::string gipLicenseManager::getFolderName() {
	return foldername;
}

std::string gipLicenseManager::getGlistEnginePath() {
	return glistenginepath;
}

std::string gipLicenseManager::getGlistProductId() {
	return glistproductid;
}

bool gipLicenseManager::saveLicenseUserName(std::string userName) {
	licenseusername = userName;

	HKEY h1, h2;
	bool avail = isLicenseAvailable();
	if(!avail) gWindows::createRegistryKey(&h1, fullpath);
	gWindows::openRegistryKey(&h2, fullpath);
	gWindows::setRegistryValue(h2, "User", licenseusername);
	gWindows::closeRegistryKey(h2);
	if(!avail) gWindows::closeRegistryKey(h1);

	return true;
}

std::string gipLicenseManager::readLicenseUserName() {
	HKEY h1;
	gWindows::openRegistryKey(&h1, fullpath);
	licenseusername = gWindows::getRegistryValue("User", fullpath);
	gWindows::closeRegistryKey(h1);
	return licenseusername;
}

std::string gipLicenseManager::getLicenseUserName() {
	return licenseusername;
}

bool gipLicenseManager::saveLicenseOwnerName(std::string ownerName) {
	licenseownername = ownerName;

	HKEY h1, h2;
	bool avail = isLicenseAvailable();
	if(!avail) gWindows::createRegistryKey(&h1, fullpath);
	gWindows::openRegistryKey(&h2, fullpath);
	gWindows::setRegistryValue(h2, "Owner", licenseownername);
	gWindows::closeRegistryKey(h2);
	if(!avail) gWindows::closeRegistryKey(h1);

	return true;
}

std::string gipLicenseManager::readLicenseOwnerName() {
	HKEY h1;
	gWindows::openRegistryKey(&h1, fullpath);
	licenseownername = gWindows::getRegistryValue("Owner", fullpath);
	gWindows::closeRegistryKey(h1);
	return licenseownername;
}

std::string gipLicenseManager::getLicenseOwnerName() {
	return licenseownername;
}

bool gipLicenseManager::saveLicense(std::string licenseStr) {
	HKEY h1, h2;
	bool avail = isLicenseAvailable();
	if(!avail) gWindows::createRegistryKey(&h1, fullpath);
	gWindows::openRegistryKey(&h2, fullpath);
	gWindows::setRegistryValue(h2, "License", licenseStr);
	gWindows::closeRegistryKey(h2);
	if(!avail) gWindows::closeRegistryKey(h1);

	HKEY h3, h4;
	if(!avail) gWindows::createRegistryKey(&h3, glistenginepath);
	gWindows::openRegistryKey(&h4, glistenginepath);
	gWindows::setRegistryValue(h4, "is_installed", "1");
	gWindows::closeRegistryKey(h4);
	if(!avail) gWindows::closeRegistryKey(h3);

	HKEY h5, h6;
	gWindows::createRegistryKey(&h5, glistenginepath2);
	gWindows::openRegistryKey(&h6, glistenginepath2);
	gWindows::setRegistryValue(h6, glistcompileid + glistcompileidabbr, "1");
	gWindows::setRegistryValue(h6, "is_engine_installed", "1");
	gWindows::closeRegistryKey(h6);
	gWindows::closeRegistryKey(h5);

	return true;
}

std::string gipLicenseManager::readLicense() {
	HKEY h1;
	gWindows::openRegistryKey(&h1, fullpath);
	std::string lic = gWindows::getRegistryValue("License", fullpath);
	gWindows::closeRegistryKey(h1);
	return lic;
}

int gipLicenseManager::loadLicense() {
	bool isavail = isLicenseAvailable();
	bool isbefore = isInstalledBefore();
//	gLogi("LicenseManager") << "is License available:" << isavail;
//	gLogi("LicenseManager") << "is Installed Before:" << isbefore;

	if(isavail && isbefore) {
//		gLogi("LicenseManager") << "License avaliable";
		std::string licread = readLicense();
		if(!isLicenseValid(licread)) {
			gLoge("LicenseManager") << "License is not valid!";
			return LOADING_INVALID;
		}

		if(isLicenseExpired(licread)) {
			gLoge("LicenseManager") << "The license is expired!";
			isexpired = true;
			return LOADING_EXPIRED;
		}
	} else if(!isavail && !isbefore) {
		//std::string license = generateTrialLicense();
		std::string license = generateLicense(LICENSE_FREE_OFFLINE);
		int licensetype = getLicenseType(license);
		std::string licenseenddate = getLicenseEndDate(license);
		std::string licensename = getLicenseName(licensetype);
//		gLogi("LicenseManager") << "Generated Trial License:" << license;
//		gLogi("LicenseManager") << "is License Valid:" << isLicenseValid(license);
//		gLogi("LicenseManager") << "is License Expired:" << isLicenseExpired();
//		gLogi("LicenseManager") << "License Type:" << licensetype;
//		gLogi("LicenseManager") << "License Name:" << licensename;
//		gLogi("LicenseManager") << "License End Date:" << licenseenddate;
		saveLicense(license);
		saveLicenseOwnerName("");
		loadLicense();
		return LOADING_NEWTRIAL;
	} else if(!isavail || !isbefore) {
		gLogi("LicenseManager") << "License deleted";
		return LOADING_DELETED;
	}

	license = readLicense();
	licenseownername = readLicenseOwnerName();
	licenseusername = readLicenseUserName();
	setLicenseEndDate(getLicenseEndDate(license));
	setLicenseType(getLicenseType(license));
	isexpired = getCurrentDateInt() > getLicenseEndDateInt();
	if(getLicenseDurationType(getLicenseType(license)) == DURATIONTYPE_INFINITE) isexpired = false;
	saveGlistCompileId();
//	gLogi("LicenseManager") << "License loaded:" << isLicenseLoaded();
//	gLogi("LicenseManager") << "License:" << getLicense();
//	gLogi("LicenseManager") << "License Type:" << getLicenseType();
//	gLogi("LicenseManager") << "License End Date:" << getLicenseEndDate();
	return LOADING_OK;
}

std::string gipLicenseManager::getLicense() {
	return license;
}

bool gipLicenseManager::isLicenseAvailable() {
	std::string res = gWindows::getRegistryValue("License", fullpath);
	return res != "";
}

bool gipLicenseManager::isInstalledBefore() {
	return (gToInt(gWindows::getRegistryValue("is_installed", glistenginepath)) == 1 ||
			gToInt(gWindows::getRegistryValue(glistcompileid + glistcompileidabbr, glistenginepath2)) == 1);
}

bool gipLicenseManager::isLicenseLoaded() {
	return license != "";
}

void gipLicenseManager::setLicenseExpired(bool isExpired) {
	isexpired = isExpired;
}

bool gipLicenseManager::isLicenseExpired() {
	return isexpired;
}

bool gipLicenseManager::isLicenseExpired(std::string licenseStr) {
	if(getLicenseDurationType(getLicenseType(licenseStr)) == DURATIONTYPE_INFINITE) return false;
	return getCurrentDateInt() > getLicenseEndDateInt(licenseStr);
}

int gipLicenseManager::getLetterNum() {
	return letternum;
}

void gipLicenseManager::setOrderingLetterNo(int orderingLetterNo) {
	olno = orderingLetterNo;
}

void gipLicenseManager::setAdditionLetterNo(int additionLetterNo) {
	alno = additionLetterNo;
}

void gipLicenseManager::setWordOrderLetterNo(int wordLetterNo) {
	wlno = wordLetterNo;
}

int gipLicenseManager::getOrderingLetterNo() {
	return olno;
}

int gipLicenseManager::getAdditionLetterNo() {
	return alno;
}

int gipLicenseManager::getWordOrderLetterNo() {
	return wlno;
}

void gipLicenseManager::setOrderingSchemeNo(int orderingSchemeNo) {
	osno = orderingSchemeNo;
}

void gipLicenseManager::setAdditionSchemeNo(int additionSchemeNo) {
	asno = additionSchemeNo;
}

void gipLicenseManager::setWordOrderSchemeNo(int wordOrderSchemeNo) {
	wsno = wordOrderSchemeNo;
}

int gipLicenseManager::getOrderingSchemeNo() {
	return osno;
}

int gipLicenseManager::getAdditionSchemeNo() {
	return asno;
}

int gipLicenseManager::getWordOrderSchemeNo() {
	return wsno;
}

void gipLicenseManager::setLicenseEndDate(std::string date) {
	licenseenddate = date;
	isinfinite = getLicenseEndDateInt() > getMaxTrialDateInt();
	wordlist[0] = licenseenddate;
	generateWordScheme();
}

std::string gipLicenseManager::getLicenseEndDate() {
	return licenseenddate;
}

int gipLicenseManager::getLicenseEndDateInt() {
	return gToInt(licenseenddate);
}

bool gipLicenseManager::isInfinite() {
	return isinfinite;
}

int gipLicenseManager::getCurrentDateInt() {
	std::string year = gToStr(gGetYear());
	if(year.length() > 2) year = year.substr(year.length() - 2, 2);
	return gToInt(year) * 10000 + gGetMonth() * 100 + gGetDay();
}

std::string gipLicenseManager::getCurrentDate() {
	return gToStr(getCurrentDateInt());
}

std::string gipLicenseManager::getDate(int dayNum) {
	time_t curr;
	time(&curr);
    curr += (86400 * dayNum);

    std::string y = gToStr(std::localtime(&curr)->tm_year - 100);
    std::string m = gToStr(std::localtime(&curr)->tm_mon + 1);
    if(m.length() == 1) m = "0" + m;
    std::string d = gToStr(std::localtime(&curr)->tm_mday);
    if(d.length() == 1) d = "0" + d;

    return y + m + d;
}

void gipLicenseManager::setMaxTrialDate(std::string date) {
	maxtrialdate = date;
	isinfinite = getLicenseEndDateInt() > getMaxTrialDateInt();
}

std::string gipLicenseManager::getMaxTrialDate() {
	return maxtrialdate;
}

int gipLicenseManager::getMaxTrialDateInt() {
	return gToInt(maxtrialdate);
}

void gipLicenseManager::setLicenseType(int type) {
	licensetype = type;
	wordlist[1] = gToStr(licensetype);
	generateWordScheme();
}

int gipLicenseManager::getLicenseType() {
	return licensetype;
}

std::string gipLicenseManager::getLicenseName(int licenseType) {
	return licensename[licenseType];
}

std::string gipLicenseManager::generateAppUID() {
	std::string wuid = gToUpper(gAES::encodeMD5("L" + gWindows::getMachineGUID()));
	std::string auid = "";
	int cnum = 0;
	int tno = -1;
	int wlength = wuid.length();
	do {
		tno++;
		if(getCharNo(wuid[tno]) != -1) {
			auid += wuid[tno];
			cnum++;
		}
	} while(cnum < 5);

	cnum = 0;
	tno = wlength;
	do {
		tno--;
		if(getCharNo(wuid[tno]) != -1) {
			auid += wuid[tno];
			cnum++;
		}
	} while(cnum < 5);

	appguid = gToUpper(auid);
	return appguid;
}

void gipLicenseManager::setAppGUID(std::string appGUID) {
	appguid = appGUID;
}

std::string gipLicenseManager::getAppUID() {
	return appguid;
}

void gipLicenseManager::setSecretWord(std::string secretWord, std::string secretWord2) {
	secretword = secretWord;
	wordlist[2] = secretWord;
	secretword2 = secretWord2;
	wordlist[3] = secretWord2;
	generateWordScheme();
}

std::string gipLicenseManager::getSecretWord() {
	return secretword;
}

std::string gipLicenseManager::getSecretWord2() {
	return secretword2;
}

std::string gipLicenseManager::generateTrialLicense() {
	return generateLicense(LICENSE_TRIAL_OFFLINE);
}

std::string gipLicenseManager::generateLicense(int licenseType) {
	int daynum[] = {0, 7, 365, 365, 1461, 30, 30, 30, 30, 17532, 17532, 17532, 17532, 17532};
	std::string lic = std::string(letternum, '*');
	std::string enddate = getDate(daynum[licenseType]);
	if(getLicenseDurationType(licenseType) == DURATIONTYPE_INFINITE) enddate = "991231";
	osno = (int)gRandom(orderingscheme.size());
	lic[olno] = chars[osno];
	asno = (int)gRandom(additionscheme.size());
	lic[alno] = chars[asno];
	wsno = (int)gRandom(wordscheme.size());
	lic[wlno] = chars[wsno];
	std::string sc2 = secretword2;
	if(getLicenseCheckType(licenseType) == CHECKTYPE_OFFLINE) sc2 = appguid;
	std::vector<std::string> wlc = {enddate, gToStr(licenseType), secretword, sc2};
	std::string wsc = wlc[wordorder[wsno][0]] + wlc[wordorder[wsno][1]] + wlc[wordorder[wsno][2]] + wlc[wordorder[wsno][3]];

	int l0 = gToInt(enddate.substr(0, 1)) + additionscheme[asno][0];
	int l1 = gToInt(enddate.substr(1, 1)) + additionscheme[asno][1];
	int l2 = gToInt(enddate.substr(2, 2)) + additionscheme[asno][2];
	int l3 = gToInt(enddate.substr(4, 2)) + additionscheme[asno][3];
	int l4 = licenseType + additionscheme[asno][4];
	if(l0 >= charnum) l0 -= charnum;
	if(l1 >= charnum) l1 -= charnum;
	if(l2 >= charnum) l2 -= charnum;
	if(l3 >= charnum) l3 -= charnum;
	if(l4 >= charnum) l4 -= charnum;
	lic[orderingscheme[osno][0]] = chars[l0];
	lic[orderingscheme[osno][1]] = chars[l1];
	lic[orderingscheme[osno][2]] = chars[l2];
	lic[orderingscheme[osno][3]] = chars[l3];
	lic[orderingscheme[osno][4]] = chars[l4];

	std::string md5 = gAES::encodeMD5(wsc);
//	gLogi("LM") << "MD5 1:" << md5;

	int mno = md5startno - 1;
	for(int i = 0; i < letternum; i++) {
		if(lic[i] != '*') continue;
		char sc;
		do {
			mno++;
			sc = md5[mno];
		} while(getControlCharNo(sc) == -1);
		lic[i] = gToUpper(std::string(1, sc))[0];
	}

	std::string finallic = "";
	for(int i = 0; i < letternum; i += stepnum) {
		finallic += lic.substr(i, stepnum);
		if(i < letternum - stepnum) finallic += "-";
	}
	return finallic;
}

bool gipLicenseManager::isLicenseValid(std::string licenseStr) {
	if(licenseStr.length() != fulllength) return false;

	std::string lic = "";
	for(int i = 0; i < licenseStr.size(); i += stepnum + 1) lic += licenseStr.substr(i, stepnum);

	int oschemeno = getCharNo(lic[olno]);
	std::vector<int> oscheme = orderingscheme[oschemeno];

	int aschemeno = getCharNo(lic[alno]);
	std::vector<int> ascheme = additionscheme[aschemeno];

	int wschemeno = getCharNo(lic[wlno]);
	std::vector<std::string> wscheme = wordscheme[wschemeno];

	int ldate0 = getCharNo(lic[oscheme[0]]) - ascheme[0];
	if(ldate0 < 0) ldate0 += charnum;
	int ldate1 = getCharNo(lic[oscheme[1]]) - ascheme[1];
	if(ldate1 < 0) ldate1 += charnum;
	int ldate2 = getCharNo(lic[oscheme[2]]) - ascheme[2];
	if(ldate2 < 0) ldate2 += charnum;
	int ldate3 = getCharNo(lic[oscheme[3]]) - ascheme[3];
	if(ldate3 < 0) ldate3 += charnum;
	std::string datestr = gToStr(ldate0) + gToStr(ldate1);
	if(ldate2 < 10) datestr += "0";
	datestr += gToStr(ldate2);
	if(ldate3 < 10) datestr += "0";
	datestr += gToStr(ldate3);

	int ltype = getCharNo(lic[oscheme[4]]) - ascheme[4];
	if(ltype < 0) ltype += charnum;

	std::string sc2 = secretword2;
	if(getLicenseCheckType(ltype) == CHECKTYPE_OFFLINE) sc2 = appguid;
	std::vector<std::string> wlc = {datestr, gToStr(ltype), secretword, sc2};
	std::string wsc = wlc[wordorder[wschemeno][0]] + wlc[wordorder[wschemeno][1]] + wlc[wordorder[wschemeno][2]] + wlc[wordorder[wschemeno][3]];
	std::string md5 = gAES::encodeMD5(wsc);

	int md5no = md5startno - 1;
	bool valid = true;
	for(int i = 0; i < letternum; i++) {
		if(i == olno || i == alno || i == wlno) continue;
		if(i == oscheme[0] || i == oscheme[1] || i == oscheme[2] || i == oscheme[3] || i == oscheme[4]) continue;
		char sc;
		do {
			md5no++;
			sc = md5[md5no];
		} while(getControlCharNo(sc) == -1);
		if(lic.substr(i, 1) != gToUpper(std::string(1, sc))) {
//			gLogi("LM") << "NON VALID i:" << i << ", lic:" << lic.substr(i, 1) << ", md5no:" << md5no << ", md5:" << gToUpper(std::string(1, sc));
//			gLogi("LM") << "md5:" << md5;
			valid = false;
			break;
		}
	}

//	gLogi("LM") << "license:" << licenseStr << ", valid:" << valid << ", type:" << ltype << ", enddate:" << datestr;

	return valid;
}

int gipLicenseManager::getLicenseType(std::string licenseStr) {
	std::string lic = "";
	for(int i = 0; i < licenseStr.size(); i += stepnum + 1) lic += licenseStr.substr(i, stepnum);

	int oschemeno = getCharNo(lic[olno]);
	std::vector<int> oscheme = orderingscheme[oschemeno];

	int aschemeno = getCharNo(lic[alno]);
	std::vector<int> ascheme = additionscheme[aschemeno];

	int ltype = getCharNo(lic[oscheme[4]]) - ascheme[4];
	if(ltype < 0) ltype += charnum;

	return ltype;
}

std::string gipLicenseManager::getLicenseEndDate(std::string licenseStr) {
	std::string lic = "";
	for(int i = 0; i < licenseStr.size(); i += stepnum + 1) lic += licenseStr.substr(i, stepnum);

	int oschemeno = getCharNo(lic[olno]);
	std::vector<int> oscheme = orderingscheme[oschemeno];

	int aschemeno = getCharNo(lic[alno]);
	std::vector<int> ascheme = additionscheme[aschemeno];

	int ldate0 = getCharNo(lic[oscheme[0]]) - ascheme[0];
	if(ldate0 < 0) ldate0 += charnum;
	int ldate1 = getCharNo(lic[oscheme[1]]) - ascheme[1];
	if(ldate1 < 0) ldate1 += charnum;
	int ldate2 = getCharNo(lic[oscheme[2]]) - ascheme[2];
	if(ldate2 < 0) ldate2 += charnum;
	int ldate3 = getCharNo(lic[oscheme[3]]) - ascheme[3];
	if(ldate3 < 0) ldate3 += charnum;
	std::string datestr = gToStr(ldate0) + gToStr(ldate1);
	if(ldate2 < 10) datestr += "0";
	datestr += gToStr(ldate2);
	if(ldate3 < 10) datestr += "0";
	datestr += gToStr(ldate3);

	return datestr;
}

int gipLicenseManager::getLicenseEndDateInt(std::string licenseStr) {
	return gToInt(getLicenseEndDate(licenseStr));
}

int gipLicenseManager::getLicenseCheckType(int licenseType) {
	return licensechecktype[licenseType];
}

int gipLicenseManager::getLicenseDurationType(int licenseType) {
	return licensedurationtype[licenseType];
}

int gipLicenseManager::getLicenseRenewalType(int licenseType) {
	return licenserenewaltype[licenseType];
}

int gipLicenseManager::getLicenseUsageType(int licenseType) {
	return licenseusagetype[licenseType];
}

char gipLicenseManager::getChar(int charNo) {
	if(charNo >= charnum || charNo < 0) return 0;
	return chars[charNo];
}

int gipLicenseManager::getCharNo(char c) {
	int n = -1;
	for(int i = 0; i < charnum; i++) {
		if(chars[i] == c) {
			n = i;
			break;
		}
	}
	return n;
}

int gipLicenseManager::getControlCharNo(char c) {
	int n = -1;
	for(int i = 0; i < controlchars.length(); i++) {
		if(controlchars[i] == c) {
			n = i;
			break;
		}
	}
	return n;
}


// HELPERS

void gipLicenseManager::getOrderingScheme() {
	// 0-15
	orderingscheme = {
			{17, 2, 8, 9, 16}, {13, 15, 1, 2, 5}, {1, 4, 0, 19, 16}, {18, 4, 1, 19, 8},
			{5, 12, 0, 8, 16}, {9, 18, 15, 11, 16}, {19, 4, 5, 15, 3}, {1, 19, 11, 17, 7},
			{19, 9, 0, 13, 4}, {8, 15, 2, 1, 17}, {15, 0, 18, 7, 17}, {16, 13, 3, 9, 2},
			{0, 2, 19, 13, 8}, {13, 1, 5, 18, 9}, {13, 17, 19, 0, 11}, {18, 4, 7, 9, 0},
			{18, 16, 2, 0, 11}, {1, 7, 3, 13, 11}, {9, 13, 2, 16, 5}, {19, 15, 7, 16, 0},
			{2, 15, 17, 1, 13}, {0, 2, 16, 1, 9}, {15, 11, 16, 5, 7}, {3, 17, 4, 16, 15},
			{9, 0, 19, 7, 15}, {2, 9, 4, 18, 11}, {18, 3, 13, 0, 9}, {19, 2, 16, 15, 11},
			{19, 16, 12, 1, 4}, {19, 18, 2, 13, 12}, {4, 3, 5, 12, 15}, {16, 1, 12, 18, 11}
	};
}

void gipLicenseManager::getAdditionScheme() {
	additionscheme = {
			{6, 1, 23, 4, 19}, {3, 14, 22, 18, 26}, {7, 3, 5, 4, 25}, {12, 25, 15, 5, 13},
			{19, 7, 20, 14, 19}, {18, 23, 18, 9, 11}, {28, 22, 23, 15, 11}, {22, 2, 17, 21, 18},
			{24, 17, 8, 8, 22}, {15, 4, 14, 22, 20}, {21, 2, 8, 9, 14}, {24, 3, 17, 24, 10},
			{10, 10, 29, 25, 22}, {27, 22, 2, 24, 9}, {9, 26, 5, 21, 6}, {6, 15, 8, 27, 30},
			{19, 18, 20, 6, 29}, {2, 12, 26, 22, 18}, {30, 18, 17, 15, 7}, {23, 22, 18, 4, 22},
			{13, 29, 13, 11, 12}, {13, 26, 19, 21, 2}, {15, 2, 16, 14, 23}, {28, 27, 10, 10, 9},
			{4, 4, 8, 30, 10}, {2, 11, 25, 6, 17}, {24, 6, 21, 15, 23}, {11, 30, 14, 4, 4},
			{25, 20, 27, 6, 8}, {4, 23, 6, 14, 1}, {23, 29, 21, 8, 15}, {4, 7, 12, 16, 20}
	};
}

void gipLicenseManager::generateWordScheme() {
	wordorder = {
			{0, 1, 2, 3},
			{0, 1, 3, 2},
			{0, 2, 1, 3},
			{0, 2, 3, 1},
			{0, 3, 1, 2},
			{0, 3, 2, 1},

			{1, 0, 2, 3},
			{1, 0, 3, 2},
			{1, 2, 0, 3},
			{1, 2, 3, 0},
			{1, 3, 0, 2},
			{1, 3, 2, 0},

			{2, 0, 1, 3},
			{2, 0, 3, 1},
			{2, 1, 0, 3},
			{2, 1, 3, 0},
			{2, 3, 0, 1},
			{2, 3, 1, 0},

			{3, 0, 1, 2},
			{3, 0, 2, 1},
			{3, 1, 0, 2},
			{3, 1, 2, 0},
			{3, 2, 0, 1},
			{3, 2, 1, 0}
	};

	wordscheme = {
			{wordlist[0], wordlist[1], wordlist[2], wordlist[3]},
			{wordlist[0], wordlist[1], wordlist[3], wordlist[2]},
			{wordlist[0], wordlist[2], wordlist[1], wordlist[3]},
			{wordlist[0], wordlist[2], wordlist[3], wordlist[1]},
			{wordlist[0], wordlist[3], wordlist[1], wordlist[2]},
			{wordlist[0], wordlist[3], wordlist[2], wordlist[1]},

			{wordlist[1], wordlist[0], wordlist[2], wordlist[3]},
			{wordlist[1], wordlist[0], wordlist[3], wordlist[2]},
			{wordlist[1], wordlist[2], wordlist[0], wordlist[3]},
			{wordlist[1], wordlist[2], wordlist[3], wordlist[0]},
			{wordlist[1], wordlist[3], wordlist[0], wordlist[2]},
			{wordlist[1], wordlist[3], wordlist[2], wordlist[0]},

			{wordlist[2], wordlist[0], wordlist[1], wordlist[3]},
			{wordlist[2], wordlist[0], wordlist[3], wordlist[1]},
			{wordlist[2], wordlist[1], wordlist[0], wordlist[3]},
			{wordlist[2], wordlist[1], wordlist[3], wordlist[0]},
			{wordlist[2], wordlist[3], wordlist[0], wordlist[1]},
			{wordlist[2], wordlist[3], wordlist[1], wordlist[0]},

			{wordlist[3], wordlist[0], wordlist[1], wordlist[2]},
			{wordlist[3], wordlist[0], wordlist[2], wordlist[1]},
			{wordlist[3], wordlist[1], wordlist[0], wordlist[2]},
			{wordlist[3], wordlist[1], wordlist[2], wordlist[0]},
			{wordlist[3], wordlist[2], wordlist[0], wordlist[1]},
			{wordlist[3], wordlist[2], wordlist[1], wordlist[0]}
	};
	wordstr = wordscheme[wsno][0] + wordscheme[wsno][1] + wordscheme[wsno][2] + wordscheme[wsno][3];
}


void gipLicenseManager::printoutOrderingScheme() {
	int numbernum = 5; //endDate year1, endDate year2, endDate month, endDate day, licenseType
	std::string ostr = "orderingscheme = {";
	for(int i = 0; i < orderingschemenum; i++) {
		std::string o1 = "{";
		std::vector<int> ov;
		for(int j = 0; j < numbernum; j++) {
			bool available;
			int r;
			do {
				available = false;
				r = (int)gRandom(letternum);
				if(r == olno || r == alno || r == wlno) available = true;
				if(!available) {
					for(int k = 0; k < j; k++) {
						if(r == ov[k]) {
							available = true;
							break;
						}
					}
				}
			} while(available);
			ov.push_back(r);
		}
		o1 += gToStr(ov[0]);
		for(int j = 1; j < numbernum; j++)  o1 += ", " + gToStr(ov[j]);
		o1 += "}";
		if(i < orderingschemenum - 1) o1 += ", ";
		ostr += o1;
	}
	ostr += "};";
//	gLogi("gCanvas") << ostr;
}

void gipLicenseManager::printoutAdditionScheme() {
	int numbernum = 5; //endDate year1, endDate year2, endDate month, endDate day, licenseType
	std::string ostr = "additionscheme = {";
	for(int i = 0; i < orderingschemenum; i++) {
		std::string o1 = "{";
		for(int j = 0; j < numbernum; j++) {
			o1 += gToStr(1 + (int)gRandom(charnum - 2));
			if(j < numbernum - 1) o1 += ", ";
		}
		o1 += "}";
		if(i < orderingschemenum - 1) o1 += ", ";
		ostr += o1;
	}
	ostr += "};";
//	gLogi("gCanvas") << ostr;
}

bool gipLicenseManager::saveGlistCompileId() {
	HKEY h1, h2;
	gWindows::createRegistryKey(&h1, glistenginepath2);
	gWindows::openRegistryKey(&h2, glistenginepath2);
	gWindows::setRegistryValue(h2, glistcompileid + glistcompileidabbr, "1");
	gWindows::setRegistryValue(h2, "is_engine_installed", "1");
	gWindows::closeRegistryKey(h2);
	gWindows::closeRegistryKey(h1);
	return true;
}

bool gipLicenseManager::checkGlistCompileId() {
	return gToInt(gWindows::getRegistryValue(glistcompileid + glistcompileidabbr, glistenginepath2)) == 1;
}

