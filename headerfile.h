#ifndef HEADERFILE_H
#define HEADERFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define USERNAME "z.hhassan567@gmail.com"
#define APP_PASSWORD "bpts fjpy wult ttcb"

struct UploadStatus {
    const char *data;
    size_t size;
};


struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t ReadCallback(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct UploadStatus *upload_ctx = (struct UploadStatus *)userp;

    size_t to_copy = size * nmemb;
    if (to_copy > upload_ctx->size)
        to_copy = upload_ctx->size;

    if (to_copy) {
        memcpy(ptr, upload_ctx->data, to_copy);
        upload_ctx->data += to_copy;
        upload_ctx->size -= to_copy;
        return to_copy;
    }

    return 0;
}

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        fprintf(stderr, "Error: Out of memory\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void extractValue(const char *response, const char *key, char *result) {
    const char *keyStart = strstr(response, key);

    if (keyStart) {
        keyStart += strlen(key);
       
        // Skip leading spaces and check for the starting quote
        while (*keyStart != '\0' && (*keyStart == ' ' || *keyStart == '\"' || *keyStart == '{' || *keyStart == '}')) {
            keyStart++;
        }

        const char *valueStart = keyStart;

        // Find the end of the value
        while (*valueStart != '\0' && *valueStart != ',' && *valueStart != '\"' && *valueStart != '}' && *valueStart != ' ') {
            valueStart++;
        }

        // Copy the value to the result
        strncpy(result, keyStart, valueStart - keyStart);
        result[valueStart - keyStart] = '\0';
    } else {
        fprintf(stderr, "Error: Unable to retrieve %s data.\n", key);
    }
}


void processEnvironmentData(const char *response, FILE *outputFile) {
    char latitude[20], longitude[20], temperature[20], humidity[5], weatherDescription[50], windSpeed[20], country[20], timezone[50], pressure[20], visibility[20];

    extractValue(response, "\"lat\":", latitude);
    extractValue(response, "\"lon\":", longitude);
    extractValue(response, "\"temp\":", temperature);
    extractValue(response, "\"humidity\":", humidity);
    extractValue(response, "\"description\":", weatherDescription);
    extractValue(response, "\"speed\":", windSpeed);
    extractValue(response, "\"country\":", country);
    extractValue(response, "\"timezone\":", timezone);
    extractValue(response, "\"pressure\":", pressure);
    extractValue(response, "\"visibility\":", visibility);

    fprintf(outputFile, "\nEnvironmental Data\n");
    fprintf(outputFile, "------------------\n");
    fprintf(outputFile, "Country: %s\n", country);
    fprintf(outputFile, "Time Zone: %s\n", timezone);
    fprintf(outputFile, "Latitude: %s\n", latitude);
    fprintf(outputFile, "Longitude: %s\n", longitude);
    fprintf(outputFile, "Humidity: %s\n", humidity);
    fprintf(outputFile, "Temperature: %s\n", temperature);
    fprintf(outputFile, "Wind Speed: %s\n", windSpeed);
    fprintf(outputFile, "Pressure: %s\n", pressure);
    fprintf(outputFile, "Visibility: %s\n", visibility);
}

void calculateAverage(FILE *processedFile, FILE *reportFile) {
    double sumLat = 0.0, sumLon = 0.0, sumTemp = 0.0, sumHumidity = 0.0, sumWindSpeed = 0.0, sumPressure = 0.0, sumVisibility = 0.0;
    int count = 0;

    char line[512];
    while (fgets(line, sizeof(line), processedFile) != NULL) {
        double lat, lon, temp, windSpeed, pressure, visibility;
        int humidity;

        if (sscanf(line, "Latitude: %lf", &lat) == 1) {
            sumLat += lat;
        }

        if (sscanf(line, "Longitude: %lf", &lon) == 1) {
            sumLon += lon;
        }

        if (sscanf(line, "Temperature: %lf", &temp) == 1) {
            sumTemp += temp;
        }

        if (sscanf(line, "Humidity: %d%%", &humidity) == 1) {
            sumHumidity += humidity;
        }

        if (sscanf(line, "Wind Speed: %lf", &windSpeed) == 1) {
            sumWindSpeed += windSpeed;
        }

        if (sscanf(line, "Pressure: %lf", &pressure) == 1) {
            sumPressure += pressure;
        }

        if (sscanf(line, "Visibility: %lf", &visibility) == 1) {
            sumVisibility += visibility;
        }

        count++;
    }

    if (count > 0) {
        double avgTemp = sumTemp / count;
        double avgHumidity = sumHumidity / count;
        double avgWindSpeed = sumWindSpeed / count;
        double avgPressure = sumPressure / count;
        double avgVisibility = sumVisibility / count;

        fprintf(reportFile, "\nAverage Data\n");
        fprintf(reportFile, "-------------\n");
        fprintf(reportFile, "Average Latitude: %.4f\n", sumLat / count);
        fprintf(reportFile, "Average Longitude: %.4f\n", sumLon / count);
        fprintf(reportFile, "Average Temperature: %.2f degrees Celsius\n", avgTemp);
        fprintf(reportFile, "Average Humidity: %.2f%%\n", avgHumidity);
        fprintf(reportFile, "Average Wind Speed: %.2f m/s\n", avgWindSpeed);
        fprintf(reportFile, "Average Pressure: %.2f hPa\n", avgPressure);
        fprintf(reportFile, "Average Visibility: %.2f meters\n", avgVisibility);

        // Compare average values with standard weather conditions
        fprintf(reportFile, "\nWeather Comparison\n");
        fprintf(reportFile, "-------------------\n");

        if (avgTemp > 25.0) {
            fprintf(reportFile, "   - The temperature is higher than usual.\n");
        } else if (avgTemp < 10.0) {
            fprintf(reportFile, "   - The temperature is lower than usual.\n");
        } else {
            fprintf(reportFile, "   - The temperature is within a normal range.\n");
        }

        if (avgHumidity > 80.0) {
            fprintf(reportFile, "   - It is humid, consider staying hydrated.\n");
        } else if (avgHumidity < 30.0) {
            fprintf(reportFile, "   - It is dry, consider moisturizing.\n");
        } else {
            fprintf(reportFile, "   - Humidity is within a comfortable range.\n");
        }

        if (avgWindSpeed > 1.2 || avgWindSpeed < 0.8) {
            fprintf(reportFile, "   - Wind speed is outside the normal range.\n");
        } else {
            fprintf(reportFile, "   - Wind speed is within a normal range.\n");
        }

        if (avgPressure > 1013.25 || avgPressure < 1000.0) {
            fprintf(reportFile, "   - There might be a change in atmospheric pressure.\n");
        } else {
            fprintf(reportFile, "   - Atmospheric pressure is stable.\n");
        }

        if (avgVisibility < 1000.0 || avgVisibility > 5000.0) {
            fprintf(reportFile, "   - Visibility is either low or high, be cautious.\n");
        } else {
            fprintf(reportFile, "   - Visibility is normal.\n");
        }

        fprintf(reportFile, "Weather Comparison Complete\n");
    } else {
        fprintf(reportFile, "No data available for calculation\n");
    }
}


void analyze(const char *response, FILE *reportFile) {
    char temperature[20], humidity[5], windSpeed[20], pressure[20], visibility[20];

    extractValue(response, "\"temp\":", temperature);
    extractValue(response, "\"humidity\":", humidity);
    extractValue(response, "\"speed\":", windSpeed);
    extractValue(response, "\"pressure\":", pressure);
    extractValue(response, "\"visibility\":", visibility);

    double temp = atof(temperature);
    int hum = atoi(humidity);
    double wind = atof(windSpeed);
    double press = atof(pressure);
    double vis = atof(visibility);

    fprintf(reportFile, "\nWeather Analysis Report\n");
    fprintf(reportFile, "------------------------\n");

    fprintf(reportFile, "Temperature: %.2f degrees Celsius\n", temp);
    fprintf(reportFile, "Humidity: %d%%\n", hum);
    fprintf(reportFile, "Wind Speed: %s m/s\n", windSpeed);
    fprintf(reportFile, "Pressure: %.2f hPa\n", press);
    fprintf(reportFile, "Visibility: %.2f meters\n", vis);

    if (temp > 25.0) {
        fprintf(reportFile, "   - It feels quite warm with a high temperature.\n");
    } else if (temp < 10.0) {
        fprintf(reportFile, "   - It feels cold with a low temperature.\n");
    } else {
        fprintf(reportFile, "   - The temperature is within a comfortable range.\n");
    }

    if (hum > 80) {
        fprintf(reportFile, "   - It feels humid, consider staying hydrated.\n");
    } else if (hum < 30) {
        fprintf(reportFile, "   - It feels dry, consider moisturizing.\n");
    } else {
        fprintf(reportFile, "   - Humidity is within a comfortable range.\n");
    }

    if (wind > 1.2 || wind < 0.8) {
        fprintf(reportFile, "   - It's windy, hold on to your hats!\n");
    } else {
        fprintf(reportFile, "   - Wind speed is within a normal range.\n");
    }

    if (press > 1013.25 || press < 1000.0) {
        fprintf(reportFile, "   - There might be a change in atmospheric pressure.\n");
    } else {
        fprintf(reportFile, "   - Atmospheric pressure is stable.\n");
    }

    if (vis < 1000.0 || vis > 5000.0) {
        fprintf(reportFile, "   - Visibility is either low or high, be cautious.\n");
    } else {
        fprintf(reportFile, "   - Visibility is normal.\n");
    }

    fprintf(reportFile, "Analysis Report Complete\n");
}

void saveToFile(const char *data, const char *filename) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file %s for writing\n", filename);
        return;
    }

    if (fprintf(file, "%s", data) < 0) {
        fprintf(stderr, "Error: Unable to write data to file %s\n", filename);
    }

    fclose(file);
}



char *readFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file %s for reading\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *content = malloc(file_size + 1);
    if (content == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for reading file %s\n", filename);
        fclose(file);
        return NULL;
    }

    if (fread(content, 1, file_size, file) != file_size) {
        fprintf(stderr, "Error: Unable to read content from file %s\n", filename);
        free(content);
        fclose(file);
        return NULL;
    }

    content[file_size] = '\0';

    fclose(file);
    return content;
}

void readAndPrintAllFiles() {
    const char *raw_filename = "raw.txt";
    const char *processed_filename = "processed.txt";
    const char *report_filename = "report.txt";

    char *raw_data = readFromFile(raw_filename);
    if (raw_data != NULL) {
        printf("Raw Data from %s:\n%s\n", raw_filename, raw_data);
        free(raw_data);
    } else {
        fprintf(stderr, "Error: Unable to read raw data from file %s\n", raw_filename);
    }

    char *processed_data = readFromFile(processed_filename);
    if (processed_data != NULL) {
        printf("Processed Data from %s:\n%s\n", processed_filename, processed_data);
        free(processed_data);
    } else {
        fprintf(stderr, "Error: Unable to read processed data from file %s\n", processed_filename);
    }

    char *report_data = readFromFile(report_filename);
    if (report_data != NULL) {
        printf("Analysis Report from %s:\n%s\n", report_filename, report_data);
        free(report_data);
    } else {
        fprintf(stderr, "Error: Unable to read analysis report from file %s\n", report_filename);
    }
}


void send_email() {
   CURL *emailCurl = curl_easy_init();

    if (emailCurl) {
        const char *from = "z.hhassan567@gmail.com";
        const char *to = "z.hhassan567@gmail.com";
        const char *subject = "Weather Report";
        const char *body  = readFromFile("report.txt");
       
        char payload[2048];
        snprintf(payload, sizeof(payload), "From: %s\r\nTo: %s\r\nSubject: %s\r\n\r\n%s", from, to, subject, body);

        struct UploadStatus upload_ctx = {payload, strlen(payload)};

        curl_easy_setopt(emailCurl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
        curl_easy_setopt(emailCurl, CURLOPT_USERNAME, "z.hhassan567@gmail.com");
        curl_easy_setopt(emailCurl, CURLOPT_PASSWORD, "bpts fjpy wult ttcb");
        curl_easy_setopt(emailCurl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(emailCurl, CURLOPT_MAIL_FROM, from);

        struct curl_slist *recipients = NULL;
        recipients = curl_slist_append(recipients, to);
        curl_easy_setopt(emailCurl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(emailCurl, CURLOPT_USERNAME, from);
        curl_easy_setopt(emailCurl, CURLOPT_PASSWORD, APP_PASSWORD);

        curl_easy_setopt(emailCurl, CURLOPT_READFUNCTION, ReadCallback);
        curl_easy_setopt(emailCurl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(emailCurl, CURLOPT_UPLOAD, 1L);

        CURLcode emailRes = curl_easy_perform(emailCurl);

        if (emailRes != CURLE_OK) {
            fprintf(stderr, "Email sending failed: %s\n", curl_easy_strerror(emailRes));
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(emailCurl);
        
    }

}
#endif // HEADERFILE_H
