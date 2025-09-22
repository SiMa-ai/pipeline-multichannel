/*
 * Copyright 2025 SiMa.ai
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <aggregator_old/agg_template.h>
#include <opencv2/opencv.hpp>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <ctime>
#include <cstring>
#include <span>
#include <nlohmann/json.hpp>
struct Detection
{
    float conf;
    cv::Rect bbox;
    int cls;
};
struct BoundingBoxOut
{
    int _x;
    int _y;
    int _w;
    int _h;
    float _score;
    int _class_id;
};
static int curr = 0;

UserContext::UserContext(nlohmann::json *json) : parser(json)
{
    // In the constructor we do not update parameters; they are updated per stream in run()
}

UserContext::~UserContext() {}

const char *UserContext::getSinkPadTemplateCaps() { return "ANY"; }
const char *UserContext::getSinkPadCaps() { return "ANY"; }
const char *UserContext::getSrcPadTemplateCaps() { return "ANY"; }
const char *UserContext::getSrcCaps() { return "ANY"; }
bool UserContext::fixateSrcCaps(GstCaps **) { return true; }
bool UserContext::parseSinkCaps(GstCaps *) { return true; }

constexpr int width = 640;
constexpr int height = 480;
constexpr size_t yuv_size = 1.5 * width * height; // 1382400
constexpr size_t bbox_size = 580;
constexpr size_t total_size = yuv_size + bbox_size;
int test;
bool is_read_json = false;
std::vector<std::string> labels;
void read_json(nlohmann::json *parser, const std::string &stream_id = "")
{
    if (!parser)
    {
        std::cerr << "Error: JSON parser is null" << std::endl;
        return;
    }
    if (parser->contains("just_test"))
    {
        test = (*parser)["just_test"].get<int>();
        // std::cout << "test: " << test << std::endl;
    }
    else
    {
        std::cerr << "Error: JSON does not contain 'just_test'" << std::endl;
    }
    if (parser->contains("labels"))
    {
        labels = (*parser)["labels"].get<std::vector<std::string>>();
        // std::cout << "Labels: ";
        // for (const auto &label : labels)
        // {
        //     std::cout << label << " ";
        // }
        // std::cout << std::endl;
    }
    else
    {
        std::cerr << "Error: JSON does not contain 'labels'" << std::endl;
    }
    is_read_json = true;
}
// void read_label
void UserContext::run(std::vector<Input> &input, std::span<uint8_t> output)
{
    // Write the 1382400 bytes to output buffer in the beginning
    // std::memcpy(output.data(), input[1].getData().data(), yuv_size);

    // // appended the rest 580 bytes after the 1382400 bytes denotes metadata of the frame
    // std::memcpy(output.data() + yuv_size, input[0].getData().data(), bbox_size);
    // std::cout << "Input size: " << input.size() << std::endl;
    if (!is_read_json)
        read_json(parser, input[0].getMetadata().stream_id);
    uint8_t *boxData = input[0].getData().data();
    int num_boxes = *reinterpret_cast<int *>(boxData);
    // std::cout << "Number of boxes: " << num_boxes << std::endl;
    boxData += sizeof(int);
    nlohmann::json output_json;
    output_json["type"] = "object-detection";
    output_json["timestamp"] = input[0].getMetadata().timestamp;
    output_json["frame_id"] = input[0].getMetadata().stream_id;
    output_json["data"]["objects"] = nlohmann::json::array();

    for (int i = 0; i < num_boxes; ++i)
    {
        BoundingBoxOut bbox_out;
        std::memcpy(&bbox_out, boxData + i * sizeof(BoundingBoxOut), sizeof(BoundingBoxOut));
        // std::string label = bbox_out._class_id == 0 ? "Car" : "Scratch";
        std::string label = bbox_out._class_id < labels.size() ? labels[bbox_out._class_id] : "Unknown";
        // std::cout << "Label: " << label << std::endl;
        nlohmann::json obj;
        obj["id"] = "obj_" + std::to_string(i + 1);
        obj["label"] = label;
        obj["confidence"] = bbox_out._score;
        obj["bbox"] = {bbox_out._x, bbox_out._y, bbox_out._w, bbox_out._h};

        output_json["data"]["objects"].push_back(obj);
    }
    // std::cout << output_json.dump(4) << std::endl;
    std::string json_str = output_json.dump();
    if (json_str.size() < output.size())
    {
        std::memset(output.data() + json_str.size(), 0, output.size() - json_str.size());
    }
    std::memcpy(output.data(), json_str.data(), std::min(output.size(), json_str.size()));
}
