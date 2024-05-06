#include <iostream>
#include <pthread.h>
#include <vector>
#include <time.h>
#include <unistd.h>

#define SAVE_FREQUENCY 60 // seconds per video
#define NUM_VIDEOS 60

bool recording = true;

//pthread_mutex_t record_lock = PTHREAD_MUTEX_INITIALIZER;

struct id_struct {
    int id;
};


void* record(void* id)
{
    struct id_struct* x = (struct id_struct*)id;

    int sensor_id = x->id;
    std::cout << sensor_id << std::endl;
    std::string time_string;
    std::vector<std::string> months = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    time_string = months[timeinfo->tm_mon] + "_" + std::to_string(timeinfo->tm_mday)  + "_" + std::to_string(timeinfo->tm_hour) + "_" + std::to_string(timeinfo->tm_min) + "_" + std::to_string(timeinfo->tm_sec);
    time_string ="/home/computer/DataCollection/" + (sensor_id == 1 ? "NoIR/NoIR_" + time_string + ".mp4" : "Optical/Optical_" + time_string + ".mp4");
    system(("sh /home/computer/DataCollection/record_n_seconds.sh " + std::to_string(sensor_id) + " " + std::to_string(SAVE_FREQUENCY) + " " + time_string).c_str());
}

void* blink(void*) // blink the LED
{
    int i = 0;
    bool done = false;
    while (!done)
    {
        if (i % 2 == 0)
        {
            system("sh /home/computer/DataCollection/all_on.sh");
        }
        else if (i % 2 == 1)
        {
            system("sh /home/computer/DataCollection/all_off.sh");
        }
        sleep(1);
        i++;
    }


}

int main()
{
    struct id_struct id0;
    id0.id = 0;
    struct id_struct id1;
    id1.id = 1;

    pthread_t NoIR, Optical;
    pthread_t blink_thread;
    pthread_create(&blink_thread, 0, blink, 0);
    int count = 0;
    while (count < NUM_VIDEOS)
    {
        pthread_create(&NoIR, 0, record, &id0);
        pthread_create(&Optical, 0, record, &id1);
        sleep(SAVE_FREQUENCY);
        pthread_join(NoIR, 0);
        pthread_join(Optical, 0);
        count++;
        sleep(2);
    }
    pthread_join(blink_thread, 0);

    return 0;
}
