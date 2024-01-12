#include "headerfile.h"

int main() {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openweathermap.org/data/2.5/weather?lat=24.860966&lon=66.990501&appid=e94521743de4a5f4b15f1a8501b87ca4");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
        } else {
            char *raw_filename = "raw.txt";
            char *processed_filename = "processed.txt";
            char *report_filename = "report.txt";

            saveToFile(chunk.memory, raw_filename);

            FILE *processedFile = fopen(processed_filename, "a");
            if (processedFile != NULL) {
                processEnvironmentData(chunk.memory, processedFile);
                fclose(processedFile);

                // Rewind the file pointer to the beginning
                processedFile = fopen(processed_filename, "r");
                if (processedFile != NULL) {
                    FILE *reportFile = fopen(report_filename, "w");
                    if (reportFile != NULL) {
                        calculateAverage(processedFile, reportFile);
                        fclose(reportFile);
                    } else {
                        fprintf(stderr, "Error: Unable to open file %s for writing\n", report_filename);
                    }
                    fclose(processedFile);
                } else {
                    fprintf(stderr, "Error: Unable to open file %s for reading\n", processed_filename);
                }
            } else {
                fprintf(stderr, "Error: Unable to open file %s for writing\n", processed_filename);
            }

            // Option to read the text files
            int readChoice;
            printf("Do you want to read the text files? (1 for Yes, 0 for No): ");
            scanf("%d", &readChoice);

            if (readChoice == 1) {
                readAndPrintAllFiles();
            }
        }
        send_email();

        free(chunk.memory);
        curl_easy_cleanup(curl);
    }

// Send email
    
   
    // Save the report to a file
    //char *reportData = readFromFile("report.txt");
    //if (reportData != NULL) {
        // Print the report data for debugging
      //  printf("Report Data:\n%s\n", reportData);
        //sendEmail(reportData);

        //free(reportData);
    //} else {
      //  fprintf(stderr, "Error: Unable to read report data from file report.txt\n");
    
    curl_global_cleanup();
    return 0;
}
	
