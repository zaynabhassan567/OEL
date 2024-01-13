#include "headerfile.h"

#define MAX_COUNT 5

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
            char *run_times_filename = "run_time.txt";

            saveToFile(chunk.memory, raw_filename);

            FILE *runTimesFile = fopen(run_times_filename, "r");
            if (runTimesFile != NULL) {
                int count;
                fscanf(runTimesFile, "%d", &count);
                fclose(runTimesFile);

                if (count < MAX_COUNT) {
                    count++;
                } else {
                    // Reset count to 0 when it reaches MAX_COUNT
                    count = 0;
                    FILE *file1 = fopen("processed.txt", "w");
                    FILE *file2 = fopen("raw.txt", "w");
            
                }

                runTimesFile = fopen(run_times_filename, "w");
                if (runTimesFile != NULL) {
                    fprintf(runTimesFile, "%d", count);
                    fclose(runTimesFile);

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
                } else {
                    fprintf(stderr, "Error: Unable to open file %s for writing\n", run_times_filename);
                }
            } else {
                fprintf(stderr, "Error: Unable to open file %s for reading\n", run_times_filename);
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

    curl_global_cleanup();
    return 0;
}
