#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

#define API_KEY "b07024da23edbf67525c2bbe2786aafb"
#define CITY "Los%20Angeles"
#define COUNTRY "US"

int main(void){
	CURL *curl;
	CURLcode res;
	char url[512];

	// URL
	snprintf(url, sizeof(url),
		"https://api.openweathermap.org/data/2.5/weather?q=%s,%s&appid=%s&units=metric",
	CITY, COUNTRY, API_KEY);

	curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK){
			fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
		}

		curl_easy_cleanup(curl);
	}
	return 0;
}
