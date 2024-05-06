//
// Created by computer on 1/2/24.
//
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <opencv2/opencv.hpp>

#define BURST_MULTIPLE 5

/*
 * before running the script, make sure to putt all the data in the /home/connputer/CS/DroneProjecct/video/ data folders
 *
 * everything should output into /home/connputer/CS/DroneProject/images/ folders
 */

namespace fs = std::filesystem;

std::string DATA_PATH = "/home/computer/CS/DroneProject/";
std::string image_path = DATA_PATH + "im2im_labeling/images/";
std::unordered_map<std::string, int> months;


void process_and_save(cv::Mat NoIR, cv::Mat optical, cv::Mat thermal, std::string label);
int str_to_int(std::string& s);
int getSeconds(std::vector<std::string> time_stuff);
std::vector<std::string> split(std::string& s, char delim);
int time_diff(std::string& file1, std::string& file2);
std::vector<std::string> timeStuff(std::string&);
std::vector<std::pair<std::string, std::string>> getIntersection(std::vector<std::string>& noIR_files, std::vector<std::string>& optical_files);
std::pair<std::vector<std::string>, std::vector<std::string>> get_non_empty(std::vector<std::pair<std::string, std::string>>& pairs);
void sort_by_time(std::vector<std::string>& thermal_files);
void test_process_and_save(cv::Mat NoIR, cv::Mat optical, cv::Mat thermal, std::string label); // for the test set


int main()
{
    std::vector<std::string> thermal_files;
    for (const auto& file : fs::directory_iterator(DATA_PATH + "video/DataDec3/"))
    {
        thermal_files.push_back(file.path().string());
    }

    std::vector<std::string> unfiltered_files;
    for (const auto& file : fs::directory_iterator(DATA_PATH + "video/good_video2/unfiltered/"))
    {
        unfiltered_files.push_back(file.path().string());
    }

    std::vector<std::string> filtered_files;
    for (const auto& file : fs::directory_iterator(DATA_PATH + "video/good_video2/filtered/"))
    {
        filtered_files.push_back(file.path().string());
    }

    /* if (unfiltered_files.size() != filtered_files.size())
     {
         std::cout << "Amount of optical video not the same as NoIR cut-off fitler video\n" << std::endl;
         return 0;
     }*/

    std::vector<std::pair<std::string, std::string>> intersection_set = getIntersection(unfiltered_files, filtered_files);
    std::pair<std::vector<std::string>, std::vector<std::string>>  new_versions = get_non_empty(intersection_set);
    unfiltered_files = new_versions.first;
    filtered_files = new_versions.second;

    sort_by_time(unfiltered_files);
    sort_by_time(filtered_files);
    sort_by_time(thermal_files);

    cv::namedWindow("NoIR", cv::WINDOW_NORMAL);
    cv::resizeWindow("NoIR", cv::Size(1570, 2016));
    cv::namedWindow("Optical", cv::WINDOW_NORMAL);
    cv::resizeWindow("Optical", cv::Size(1570, 2016));

   // std::cout << "What frame are we starting at: ";
    int frame = 0;
   // std::cin >> frame;
    for (const auto& file : fs::directory_iterator(image_path+ "filtered/"))
    {
        frame++;
    }

    int test_frame = 0;

    for (const auto& file : fs::directory_iterator(image_path + "filtered/"))
    {
        test_frame++;
    }

    int current_thermal_file = 0;
    for (int i= 0 ; i < unfiltered_files.size(); i++) {
        // read in frames until we have the right amount
        try {
            while (current_thermal_file < thermal_files.size()-1 && time_diff(thermal_files[current_thermal_file+1], filtered_files[i]) < 0)
            {
                current_thermal_file++;
            }
            cv::VideoCapture thermal_vid;
            if (current_thermal_file == thermal_files.size()-1) // if it's the last and still greater
            {
                if (time_diff(thermal_files[current_thermal_file], filtered_files[i]) > 0) // if this file does not contain the scene
                {
                    std::cout << "Could not find a thermal video file containing the scene from " << unfiltered_files[i] << " and " << filtered_files[i] << std::endl;
                    return 0;
                }
                else
                {
                    // see if the video is long enough to contain the scene
                    int seconds = time_diff(filtered_files[i], thermal_files[current_thermal_file]);
                    int need_to_read = 25*seconds;
                    thermal_vid.open(thermal_files[current_thermal_file]);
                    int frames_in_vid = thermal_vid.get(cv::CAP_PROP_FRAME_COUNT);
                    thermal_vid.release();
                    if (need_to_read > frames_in_vid)
                    {
                        std::cout << "No thermal video found to have the scene from " << unfiltered_files[i] << std::endl;
                        return 0;
                    }
                }
            }

            std::cout << "Thermal video: " << thermal_files[current_thermal_file] << std::endl;
            std::cout << "Jetson videos:\n\tNoIR: " << unfiltered_files[i] << std::endl << "\tOptical: " << filtered_files[i] << std::endl;

            thermal_vid.open(thermal_files[current_thermal_file]);

            int seconds_to_read = time_diff(filtered_files[i], thermal_files[current_thermal_file]);
            cv::Mat thermal;
            for (int k = 0; k < 25*(seconds_to_read); k++)
            {
                thermal_vid >> thermal;
            }

            cv::VideoCapture optical_vid(filtered_files[i]);
            cv::VideoCapture noIR_vid(unfiltered_files[i]);
            cv::Mat optical, noIR;
            optical_vid >> optical;
            noIR_vid >> noIR;
            thermal_vid >> thermal;

            // align the video to be at the same time as the NoIR and optical videos
            bool thermal_empty = false;
            for (int f = 0 ; f < 233 && !thermal_empty; f++) {
                bool done = false;
                while (!done) {
                    if (thermal.empty() || noIR.empty() || optical.empty())
                    {
                        thermal_empty = true;
                        break;
                    }
                    cv::imshow("NoIR", noIR);
                    cv::imshow("Optical", optical);
                    cv::imshow("Thermal", thermal);
                    int key = cv::waitKey(0);
                    if (key == 'd')
                    {
                        thermal_vid >> thermal;
                    }
                    if (key == 'c')
                    {
                        noIR_vid >> noIR;
                    }
                    if (key == 'e')
                    {
                        optical_vid >> optical;
                    }
                    else if (key == 'q')
                    {
                        done = true;
                    }
                }
                if (thermal_empty)
                    break;

                std::vector<std::pair<cv::Mat, cv::Mat>> burst;
                std::vector<cv::Mat> burst2;
                std::vector<std::vector<cv::Mat>> test_frames_to_save(BURST_MULTIPLE);
                cv::Mat optical2, noIR2;

                for (int p = 1; p <= 5*BURST_MULTIPLE && !thermal_empty; p++)
                {
                    noIR_vid >> noIR;
                    optical_vid >> optical;
                    thermal_vid >> thermal;

                    if (thermal.empty())
                    {
                        thermal_empty = true;
                    }
                    else if (p % 5 == 0) // thermal fps is 25, optical and NoIR is 30. 6 frames of noIR/optical for every 5 frames of thermal
                    {
                        optical_vid >> optical2;
                        noIR_vid >> noIR2;
                        test_frames_to_save[(p-1)/5] = std::vector<cv::Mat>{noIR2, optical2, thermal};
                    }
                    burst.push_back(std::make_pair(noIR, optical));
                    burst2.push_back(thermal);
                }

                for (int l = 1 ; l <= 5*BURST_MULTIPLE && !thermal_empty; l++)
                {
                    process_and_save(burst[l-1].first, burst[l-1].second, burst2[l-1], std::to_string(frame));
                    frame++;
                }

                for (int k =0 ; k < BURST_MULTIPLE && !thermal_empty; k++)
                {
                    test_process_and_save(test_frames_to_save[k][0], test_frames_to_save[k][1], test_frames_to_save[k][2], std::to_string(test_frame));
                    test_frame++;
                }

            }
            optical_vid.release();
            thermal_vid.release();
            noIR_vid.release();

            system(("mv " + unfiltered_files[i] + " " + DATA_PATH + "video/good_video2/unfiltered_done_labeling/").c_str());
            system(("mv " + filtered_files[i] + " " + DATA_PATH + "video/good_video2/filtered_done_labeling/").c_str());
        } catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }

    }
    cv::destroyAllWindows();
    std::cout << "Done." << std::endl;
}

bool compare(std::string& file_1, std::string& file_2)
{
    return getSeconds(timeStuff(file_1)) < getSeconds(timeStuff(file_2));
}

void sort_by_time(std::vector<std::string>& thermal_files)
{
    std::sort(thermal_files.begin(), thermal_files.end(), compare);
}

void process_and_save(cv::Mat NoIR, cv::Mat optical, cv::Mat thermal, std::string label)
{
    // allign
    int height_dif = 895-680;
    int width_dif = 2210-2053;

    cv::Mat cropped_noIR(NoIR, cv::Rect(0, height_dif, 4032-width_dif, 3040-height_dif));
    cv::Mat cropped_optical(optical, cv::Rect(width_dif, 0, 4032-width_dif, 3040-height_dif));

    // subtract
    cv::Mat subtraction = cropped_noIR - cropped_optical;

    // save all images
    cv::imwrite(image_path + "unfiltered/NoIR_" + label + ".jpg", cropped_noIR);
    cv::imwrite(image_path + "filtered/Optical_" + label + ".jpg", cropped_optical);
    cv::imwrite(image_path + "thermal/thermal_" + label + ".jpg", thermal);
    cv::imwrite(image_path + "subtractions/subtraction_" + label + ".jpg", subtraction);
    std::cout << "Saved " << label << std::endl;
}

void test_process_and_save(cv::Mat NoIR, cv::Mat optical, cv::Mat thermal, std::string label)
{
    int height_dif = 895-680;
    int width_dif = 2210-2053;

    cv::Mat cropped_noIR(NoIR, cv::Rect(0, height_dif, 4032-width_dif, 3040-height_dif));
    cv::Mat cropped_optical(optical, cv::Rect(width_dif, 0, 4032-width_dif, 3040-height_dif));

    // subtract
    cv::Mat subtraction = cropped_noIR - cropped_optical;

    // save all images
    cv::imwrite(image_path + "test/unfiltered/NoIR_" + label + ".jpg", cropped_noIR);
    cv::imwrite(image_path + "test/filtered/Optical_" + label + ".jpg", cropped_optical);
    cv::imwrite(image_path + "test/thermal/thermal_" + label + ".jpg", thermal);
    cv::imwrite(image_path + "test/subtractions/subtraction_" + label + ".jpg", subtraction);
    std::cout << "Saved " << label << std::endl;
}


int str_to_int(std::string& s)
{
    int ct = 0;
    for (int i= 0 ; i < s.size(); i++)
    {
        ct *= 10;
        ct += s[i]-'0';
    }
    return ct;
}

// seconds after Nov 21 at 12:00 a.m.
int getSeconds(std::vector<std::string> time_stuff)
{
    //  std::cout << "got here" << std::endl;
    months["Jan"] = 0;
    months["Feb"] = 1;
    months["Mar"] = 2;
    months["Apr"] = 3;
    months["May"] = 4;
    months["Jun"] = 5;
    months["Jul"] = 6;
    months["Aug"] = 7;
    months["Sep"] = 8;
    months["Oct"] = 9;
    months["Nov"] = 10;
    months["Dec"] = 11;
//    std::cout << "got over here" << std::endl;
    int year = str_to_int(time_stuff[0])-23;
    int month = months[time_stuff[1]]-10;
    int day = str_to_int(time_stuff[2])-20;
    int hour = str_to_int(time_stuff[3]);
    int min = str_to_int(time_stuff[4]);
    int seconds = str_to_int(time_stuff[5]);
    if (day < 0)
    {
        day += 30;
        month -= 1;
    }
    if (month < 0)
    {
        month += 12;
        year -= 1;
    }

    month += 12 * year;
    day += 30*month;
    hour += 24*day;
    min += 60*hour;
    seconds += 60*min;
    return seconds;
}

std::vector<std::string> split(std::string& s, char delim)
{
    std::vector<std::string> retval;
    std::string k = "";
    for (int i=0 ; i < s.size();i++)
    {
        if (s[i]==delim)
        {
            retval.push_back(k);
            k = "";
        }
        else
        {
            k += s[i];
        }
    }
    retval.push_back(k);
    return retval;
}

std::vector<std::string> timeStuff(std::string& filename)
{
    auto s_vec = split(filename, '/');
    std::string fname = s_vec[s_vec.size()-1];
    std::string filename1 = s_vec[s_vec.size()-1].substr(0, fname.size()-4);
    auto temp = split(filename1, '_');
    return std::vector<std::string>(temp.begin()+1, temp.end());
}
int time_diff(std::string& file1, std::string& file2)
{

    std::vector<std::string> time_info1 = timeStuff(file1); // everthing but the type of light captured
    std::vector<std::string> time_info2 = timeStuff(file2);
    return getSeconds(time_info1) - getSeconds(time_info2);
}

std::vector<std::pair<std::string, std::string>> getIntersection(std::vector<std::string>& noIR_files, std::vector<std::string>& optical_files)
{
    // get the seconds for each noIR file
    // store the seconds_to_noIR_file with items (seconds +- 2, noIR_file)
    std::unordered_map<int, std::string> noIR_seconds_to_files;
    int s;
    for (std::string filename : noIR_files)
    {
        s = getSeconds(timeStuff(filename));
        for (int i=-5; i<6;i++)
        {
            noIR_seconds_to_files[s+i] = filename;
        }
    }
    // if there's an optical file +- 2 seoonds within any noIR file, add dat shit
    std::vector<std::pair<std::string, std::string>> intersection_set;

    for (std::string optical_file : optical_files)
    {
        s = getSeconds(timeStuff(optical_file));
        if (noIR_seconds_to_files[s].size() != 0)
        {
            intersection_set.push_back(std::make_pair(noIR_seconds_to_files[s], optical_file));
        }
    }

    return intersection_set;
}


std::pair<std::vector<std::string>, std::vector<std::string>> get_non_empty(std::vector<std::pair<std::string, std::string>>& pairs)
{
    std::vector<std::string> good_noIR, good_optical;
    for (std::pair<std::string, std::string> p_i : pairs)
    {
        std::ifstream in(p_i.first, std::ifstream::ate | std::ifstream::binary);
        int sz = in.tellg();
        if (sz==0)
        {
            continue;
        }
        in.close();
        in.open(p_i.second, std::ifstream::ate | std::ifstream::binary);
        sz = in.tellg();
        if (sz == 0)
        {
            continue;
        }

        good_noIR.push_back(p_i.first);
        good_optical.push_back(p_i.second);
    }

    return std::make_pair(good_noIR, good_optical);
}
