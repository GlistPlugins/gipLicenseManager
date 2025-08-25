/*
 * gipLicenseManager.h
 *
 *  Created on: Mar 11, 2023
 *      Author: Noyan Culum
*/

#ifndef SRC_GIPLICENSEMANAGER_H_
#define SRC_GIPLICENSEMANAGER_H_

#include "gBaseComponent.h"
#include "gWindows.h"


class gipLicenseManager : public gBaseComponent{
public:
	static const int licensenum = 15;

	enum {
		LICENSE_INVALID,
		LICENSE_TRIAL_OFFLINE,
		LICENSE_FREE_OFFLINE,

		LICENSE_STUDENT_1YEAR,
		LICENSE_OEM_1YEAR,
		LICENSE_ACADEMIC_4YEAR,

		LICENSE_PERSONAL_SUBSCRIPTION,
		LICENSE_BUSINESS_SUBSCRIPTION,
		LICENSE_GOVERNMENTAL_SUBSCRIPTION,
		LICENSE_OEM_SUBSCRIPTION,

		LICENSE_PERSONAL_INFINITE,
		LICENSE_BUSINESS_INFINITE,
		LICENSE_GOVERNMENTAL_INFINITE,
		LICENSE_OEM_INFINITE,

		LICENSE_INDUSTRIAL_INFINITE_OFFLINE
	};

	enum {
		CHECKTYPE_NONE,
		CHECKTYPE_OFFLINE,
		CHECKTYPE_ONLINE
	};

	enum {
		DURATIONTYPE_NONE,
		DURATIONTYPE_LIMITED,
		DURATIONTYPE_INFINITE
	};

	enum {
		RENEWALTYPE_NONE,
		RENEWALTYPE_SUBSCRIPTION,
		RENEWALTYPE_FULL
	};

	enum {
		USAGETYPE_NONE,
		USAGETYPE_TRIAL,
		USAGETYPE_ACTUAL
	};

	enum {
		LOADING_OK, LOADING_NEWTRIAL, LOADING_EXPIRED, LOADING_DELETED, LOADING_INVALID
	};

	enum {
		REGISTRYHIVE_CURRENTUSER,
		REGISTRYHIVE_LOCALMACHINE
	};

	gipLicenseManager();
	virtual ~gipLicenseManager();

	/**
	 * @param glistProductId Name of the app. This sould be a completely different name than the original app name.
	 * @param glistCompileId 6 digit string composed from only 0-9 numbers specific to the app
	 */
	bool initialize(std::string companyName, std::string softwareName, std::string glistProductId, std::string glistCompileId, std::string secretKey = "GlistEngine", std::string secretKey2 = "Software", std::string subFolder = foldername, int registryHive = REGISTRYHIVE_CURRENTUSER);
	int loadLicense();

	std::string getDeveloperCompanyName();
	std::string getSoftwareName();
	std::string getFolderName();
	std::string getGlistEnginePath();
	std::string getGlistProductId();

	bool saveLicenseOwnerName(std::string ownerName);
	std::string readLicenseOwnerName();
	std::string getLicenseOwnerName();

	bool saveLicenseUserName(std::string userName);
	std::string readLicenseUserName();
	std::string getLicenseUserName();

	bool saveLicense(std::string licenseStr);
	std::string readLicense();
	std::string getLicense();
	bool isLicenseAvailable();
	bool isInstalledBefore();
	bool isLicenseLoaded();

	void setLicenseExpired(bool isExpired);
	bool isLicenseExpired();
	bool isLicenseExpired(std::string licenseStr);

	/**
	 * Generates and returns an official and valid trial license.
	 *
	 * @return New trial license
	 */
	std::string generateTrialLicense();
	std::string generateLicense(int licenseType);

	/**
	 * Returns if the given license is a valid license.
	 *
	 * This method does checks and validates if the license
	 * is an official license.
	 *
	 * @param licenseStr License to check its type
	 *
	 * @return Is license valid and official
	 */
	bool isLicenseValid(std::string licenseStr);

	/**
	 * Returns the type of the given license.
	 *
	 * This method does not check validation of the license. One
	 * should use this function after checking the license is
	 * valid or not.
	 *
	 * @param licenseStr License to check its type
	 *
	 * @return License type
	 */
	int getLicenseType(std::string licenseStr);

	/**
	 * Returns the end date of the given license as a string.
	 *
	 * This method does not check validation of the license. One
	 * should use this function after checking the license is
	 * valid or not.
	 *
	 * @param licenseStr License to check its end date
	 *
	 * @return License end date as string
	 */
	std::string getLicenseEndDate(std::string licenseStr);

	/**
	 * Returns the end date of the given license as an integer.
	 *
	 * This method does not check validation of the license. One
	 * should use this function after checking the license is
	 * valid or not.
	 *
	 * @param licenseStr License to check its end date
	 *
	 * @return License end date as integer
	 */
	int getLicenseEndDateInt(std::string licenseStr);

	/**
	 * Returns the name of the given license type.
	 *
	 * @param licenseType License type to check its name
	 *
	 * @return License name as string
	 */
	std::string getLicenseName(int licenseType);

	int getLicenseCheckType(int licenseType);
	int getLicenseDurationType(int licenseType);
	int getLicenseRenewalType(int licenseType);
	int getLicenseUsageType(int licenseType);

	void setOrderingLetterNo(int orderingLetterNo);
	void setAdditionLetterNo(int additionLetterNo);
	void setWordOrderLetterNo(int wordLetterNo);
	int getOrderingLetterNo();
	int getAdditionLetterNo();
	int getWordOrderLetterNo();

	void setOrderingSchemeNo(int orderingSchemeNo);
	void setAdditionSchemeNo(int additionSchemeNo);
	void setWordOrderSchemeNo(int wordOrderSchemeNo);
	int getOrderingSchemeNo();
	int getAdditionSchemeNo();
	int getWordOrderSchemeNo();

	/**
	 * Sets the end date of the license
	 *
	 * @param date End date of the license in YYMMDD format as a string
	 */
	void setLicenseEndDate(std::string date);

	/**
	 * Returns the end date of the license as string
	 *
	 * @return License end date in YYMMDD format as a string
	 */
	std::string getLicenseEndDate();

	/**
	 * Returns the end date of the license as integer
	 *
	 * @return License end date in YYMMDD format as an integer
	 */
	int getLicenseEndDateInt();

	/**
	 * Returns if the license is a non expiring license or no
	 *
	 * @return The license is unexpiring or expiring
	 */
	bool isInfinite();

	int getCurrentDateInt();
	std::string getCurrentDate();
	std::string getDate(int dayNum);

	void setMaxTrialDate(std::string date);
	std::string getMaxTrialDate();
	int getMaxTrialDateInt();

	void setLicenseType(int type);
	int getLicenseType();

	std::string generateAppUID();
	void setAppGUID(std::string appGUID);
	std::string getAppUID();

	void setSecretWord(std::string secretWord, std::string secretWord2);
	std::string getSecretWord();
	std::string getSecretWord2();

	char getChar(int charNo);
	int getCharNo(char c);

private:
	static const int letternum = 20; //License letter num
	static const int stepnum = 4; // License letter group length

	static std::string foldername;

	int charnum;
	int orderingschemenum;

	std::vector<std::string> licensename;

	std::string developercompanyname;
	std::string softwarename;
	std::string fullpath;
	std::string glistenginepath, glistenginepath2;
	std::string glistproductid;
	std::string glistcompileid, glistcompileidabbr;
	int registryhive;
	HKEY registryhivekey;
	std::string license;
	bool isinfinite;
	bool isexpired;
	int fulllength;

	//letter order no
	int olno;
	int alno;
	int wlno;
	int llno;

	//selected scheme no
	int osno;
	int asno;
	int wsno;
	int lsno;

	// License properties
	// End date format: yymmdd
	std::string licenseenddate;
	std::string maxtrialdate;
	int licensetype;
	std::string appguid;
	int md5startno;
	std::string licenseownername;
	std::string licenseusername;

	//words
	std::vector<std::string> wordlist;
	std::string secretword, secretword2;
	std::string wordstr;

	//schemes
	std::vector<std::vector<int>> orderingscheme; //[36][4]
	std::vector<std::vector<int>> additionscheme; //[36][4]
	std::vector<std::vector<std::string>> wordscheme; //[24]
	std::vector<std::vector<int>> wordorder;

	int getLetterNum();

	//Scheme generators
	void getOrderingScheme();
	void getAdditionScheme();
	void generateWordScheme();

	//Helpers
	void printoutOrderingScheme();
	void printoutAdditionScheme();
	bool checkGlistCompileId();
	bool isInitializersOK(std::string companyName, std::string softwareName, std::string glistProductId, std::string glistCompileId);
	bool saveGlistCompileId();
	int getControlCharNo(char c);

	std::string chars, controlchars;
	std::vector<int> licensechecktype;
	std::vector<int> licensedurationtype;
	std::vector<int> licenserenewaltype;
	std::vector<int> licenseusagetype;
};

#endif /* SRC_GIPLICENSEMANAGER_H_ */
