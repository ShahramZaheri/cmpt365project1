#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>  // for pow
#include <map>
#include <tuple>
using namespace cv;
using namespace std;

// global variables
bool byte_order_is_II = true;
map<int, string> tag_num_to_tag_name_map;
map<string, int> tag_name_to_tag_num_map;
// ================= tag num -> tag name map ===========================
void initializing_func()
{
    // populating tag_num-> tag_name map
    tag_num_to_tag_name_map[254] = "NewSubfileType";
    tag_num_to_tag_name_map[255] = "SubfileType";
    tag_num_to_tag_name_map[256] = "ImageWidth";
    tag_num_to_tag_name_map[257] = "ImageLength";
    tag_num_to_tag_name_map[258] = "BitsPerSample";
    tag_num_to_tag_name_map[259] = "Compression";
    tag_num_to_tag_name_map[262] = "PhotometricInterpretation";
    tag_num_to_tag_name_map[263] = "Threshholding";
    tag_num_to_tag_name_map[264] = "CellWidth";
    tag_num_to_tag_name_map[265] = "CellLength";
    tag_num_to_tag_name_map[266] = "FillOrder";
    tag_num_to_tag_name_map[270] = "ImageDescription";
    tag_num_to_tag_name_map[271] = "Make";
    tag_num_to_tag_name_map[272] = "Model";
    tag_num_to_tag_name_map[273] = "StripOffsets";
    tag_num_to_tag_name_map[274] = "Orientation";
    tag_num_to_tag_name_map[277] = "SamplesPerPixel";
    tag_num_to_tag_name_map[278] = "RowsPerStrip";
    tag_num_to_tag_name_map[279] = "StripByteCounts";
    tag_num_to_tag_name_map[280] = "MinSampleValue";
    tag_num_to_tag_name_map[281] = "MaxSampleValue";
    tag_num_to_tag_name_map[282] = "XResolution";
    tag_num_to_tag_name_map[283] = "YResolution";
    tag_num_to_tag_name_map[284] = "PlanarConfiguration";
    tag_num_to_tag_name_map[288] = "FreeOffsets";
    tag_num_to_tag_name_map[289] = "FreeByteCounts";
    tag_num_to_tag_name_map[290] = "GrayResponseUnit";
    tag_num_to_tag_name_map[291] = "GrayResponseCurve";
    tag_num_to_tag_name_map[296] = "ResolutionUnit";
    tag_num_to_tag_name_map[305] = "Software";
    tag_num_to_tag_name_map[306] = "DateTime";
    tag_num_to_tag_name_map[315] = "Artist";
    tag_num_to_tag_name_map[316] = "HostComputer";
    tag_num_to_tag_name_map[320] = "ColorMap";
    tag_num_to_tag_name_map[338] = "ExtraSamples";
    tag_num_to_tag_name_map[33432] = "Copyright";
    // populating tag name -> tag num map
    for (auto item : tag_num_to_tag_name_map)
    {
        tag_name_to_tag_num_map[item.second] = item.first;
    }
}

void display_image(string img_path)
{
    //string image_path = img_path;
    Mat img = imread(img_path, IMREAD_COLOR);
    if (img.empty())
    {
        cout << "Could not read the image: " << endl;
        //return 1;
    }
    imshow("Display window", img);
    int k = waitKey(0); // Wait for a keystroke in the window
    //if (k == 's')
    //{
    //    imwrite("starry_night.png", img);
    //}
}

//int byte_vector[2] int_to_byte_converter(long long int input_int)
//{
//    vector <int> byte_vec;
//    cout << "hello world" << endl;
//    return NULL;
//}
long long int byte_to_int_converter(vector<int> bytes_vector)
{
    long long int ret = 0;
    int i = 0;
    if (byte_order_is_II)
    {
        for (auto itr = bytes_vector.begin(); itr != bytes_vector.end(); itr++)
        {
            ret = ret + *itr * pow(256, i);
            i++;
        }
    }
    else
    {
        for (auto itr = bytes_vector.begin(); itr != bytes_vector.end(); itr++)
        {
            ret = 256 * ret + *itr;
            i++;
        }

    }
    return ret;
}
int RGB_to_GrayScale(int R, int G, int B)
{
    int gray_scale_color = 0.299 * R + .587 * G + .114 * B;
    return (gray_scale_color);
}




//void read_original_file_data(string input_file_name)
//{
//    
//    
//}

void make_gray(string input_file_name, string output_file_name)
{
 
    long long int number_of_directories, IFD, num_of_bytes;
    FILE* fp;
    vector<int> data, output_Int_Vec;

    errno_t err = fopen_s(&fp, input_file_name.c_str(), "rb");
    if (err != 0)
    {
        cout << "error in opening original file " << endl;
        exit(-1);
    }
    // reading file into data vector
    int c;
    num_of_bytes = 0;
    while (1)
    {
        c = getc(fp);
        if (feof(fp))
        {
            break;
        }
        data.push_back(c);
        num_of_bytes++;
    }
    fclose(fp);
    // after reading original file into data vector, we copy data vector into output_Int_Vec
    // we will modify output_Int_Vec for grayscale
    for (auto itr = data.begin(); itr != data.end(); itr++)
        {
            output_Int_Vec.push_back(*itr);
        }
    // checking byte order (II or MM)
    if (data[0] == 77 && data[1] == 77)
    {
        byte_order_is_II = false;
    }

    // reading offset of first IFD
    vector<int> IFD_bytes_vector(data.begin()+4, data.begin()+8);
    IFD = byte_to_int_converter(IFD_bytes_vector);

    // reading number of directories
    vector<int> num_of_directories_bytes_vector(data.begin() + IFD, data.begin() + IFD + 2);
    number_of_directories = byte_to_int_converter(num_of_directories_bytes_vector);

    // reading tag  maybe need to change int to long long int
    // tag num: (tag type, tag count, tag value/offset, index of tag value/offset)
    map<long long int, tuple <long long int, long long int, long long int, long long int>> directory_map;
    for (long long int i = 0; i < number_of_directories; i++)
    {
        vector<int> tag_num_vec(data.begin() + IFD + 2 + i * 12, data.begin() + IFD + 2 + i * 12 + 2);
        vector<int> tag_type_vec(data.begin() + IFD + 2 + i * 12 + 2, data.begin() + IFD + 2 + i * 12 + 4);
        vector<int> tag_count_vec(data.begin() + IFD + 2 + i * 12 + 4, data.begin() + IFD + 2 + i * 12 + 8);
        vector<int> tag_value_offset_vec(data.begin() + IFD + 2 + i * 12 + 8, data.begin() + IFD + 2 + i * 12 + 12);
        long long int tag_value_offset_index = IFD + 2 + i * 12 + 8;
        directory_map[(int)byte_to_int_converter(tag_num_vec)] = make_tuple((int)byte_to_int_converter(tag_type_vec), (int)byte_to_int_converter(tag_count_vec), (int)byte_to_int_converter(tag_value_offset_vec), tag_value_offset_index);
    }
    // printing directory map
    for (auto item : directory_map)
    {
        cout << item.first << " ";
        cout << get<0>(item.second) << ", " << tag_num_to_tag_name_map[item.first] << ", " << get<1>(item.second) << ", " << get<2>(item.second) << ", " << get<3>(item.second)<< endl;
    }

    long long int strip_offset_value = get<2>(directory_map[tag_name_to_tag_num_map["StripOffsets"]]);
    vector <long long int> offset_of_strips;
    for (int i = 0; i < get<1>(directory_map[tag_name_to_tag_num_map["StripOffsets"]]); i++)
    {
        long long int start_index = get<2>(directory_map[tag_name_to_tag_num_map["StripOffsets"]]) + i * get<0>(directory_map[tag_name_to_tag_num_map["StripOffsets"]]);
        long long int end_index = start_index + get<0>(directory_map[tag_name_to_tag_num_map["StripOffsets"]]);
        //cout << "i= " << i << "  " << start_index << endl;
        vector<int> myvec(data.begin() + start_index, data.begin() + end_index);
        offset_of_strips.push_back(byte_to_int_converter(myvec));
    }
    
    // changing tags for grayscale
    // setting PhotometricInterpretation = 1 (black is zero)
    long long int index_of_photometric_interpretation = get<3>(directory_map[tag_name_to_tag_num_map["PhotometricInterpretation"]]);
    output_Int_Vec[index_of_photometric_interpretation] = 1;
    // setting bits per sample = 8
    /*long long int index_of_bits_per_sample = get<3>(directory_map[tag_name_to_tag_num_map["BitsPerSample"]]);
    output_Int_Vec[index_of_photometric_interpretation] = 8;*/
    
//    setting strip byte counts
    vector <long long int> gray_scale_strips_byte_count;    
    for (long long int i = 0; i < get<1>(directory_map[tag_name_to_tag_num_map["StripByteCounts"]]);i++)
    {
        vector<int> strip_byte_count_vec(data.begin() + get<2>(directory_map[tag_name_to_tag_num_map["StripByteCounts"]]) + 2 * i, data.begin() + get<2>(directory_map[tag_name_to_tag_num_map["StripByteCounts"]]) + 2*i+2);
        long long int strip_byte_count = byte_to_int_converter(strip_byte_count_vec);
       
        long long int gray_scale_strip_byte_count = strip_byte_count / 3;
        gray_scale_strips_byte_count.push_back(gray_scale_strip_byte_count);
        

        /*cout <<"gray_scale_strip_byte_count % 256 " <<gray_scale_strip_byte_count % 256 << endl;
        cout << "gray_scale_strip_byte_count/256 "<< gray_scale_strip_byte_count / 256 << endl;*/
        output_Int_Vec[get<2>(directory_map[tag_name_to_tag_num_map["StripByteCounts"]]) + 2 * i] = gray_scale_strip_byte_count % 256;
        output_Int_Vec[get<2>(directory_map[tag_name_to_tag_num_map["StripByteCounts"]]) + 2 * i+1] = gray_scale_strip_byte_count/ 256;
    }

    

//   /* for (int i = 0; i < 38; i++)
//    {
//        cout << i << endl;
//        cout << output_Int_Vec[get<2>(directory_map[tag_name_to_tag_num_map["StripByteCounts"]]) + 2 * i] << endl;
//        cout << output_Int_Vec[get<2>(directory_map[tag_name_to_tag_num_map["StripByteCounts"]]) + 2 * i + 1] << endl;
//    }*/
//
    for (int offset_num = 0; offset_num < offset_of_strips.size(); offset_num++)
    {
        for (int j = 0; j < gray_scale_strips_byte_count[offset_num]; j++)
        {
            output_Int_Vec[offset_of_strips[offset_num] + j] = RGB_to_GrayScale(data[offset_of_strips[offset_num] + 3*j], data[offset_of_strips[offset_num] + 3*j + 1], data[offset_of_strips[offset_num] + 3*j + 2]);
        }

    }
    
//    cout << "=======================================================" << endl;
//
//    for (int i = 8; i < 200; i++)
//        cout << output_Int_Vec[i] << endl;
//   
//
//    /*for (auto itr = offset_of_strips.begin(); itr != offset_of_strips.end(); itr++)
//    {
//        cout << "offset_num: " << offset_num << " offset_Val: "<< *itr << " offset byte count " << gray_scale_strips_byte_count[offset_num] << endl;
//        offset_num++;
//    }*/
//
    // changing colors to gray scale

    fstream grayfp;
    grayfp.open(output_file_name.c_str(), ios::out | ios::binary);
    vector<char> outputCharVec;
    for (auto itr = output_Int_Vec.begin(); itr != output_Int_Vec.end(); itr++)
    {
        outputCharVec.push_back(*itr);
    }
    for (auto itr = output_Int_Vec.begin(); itr != output_Int_Vec.end(); itr++)
    {
        outputCharVec.push_back(*itr);
    }
    grayfp.write(outputCharVec.data(), outputCharVec.size() * sizeof(outputCharVec[0]));
    grayfp.close();
}
int main()
{
    
    initializing_func();
    string file_names[4] = {"board.tif", "autumn.tif","balloons.tif", "lena.tif" } ;
    string gray_file_names[4] = {"board_gray.tif", "autumn_gray.tif","balloons_gray.tif", "lena_gray.tif"};
    display_image(file_names[0]);
    //read_original_file_data(file_names[0]);
    make_gray(file_names[0], gray_file_names[0]);
    display_image(gray_file_names[0]);
   
    return 0;
}