# Multi-Channel Video Inference Pipeline

[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)
![SDK](https://img.shields.io/badge/SDK-1.7.0-green.svg)
![YOLOv7](https://img.shields.io/badge/model-YOLOv7-purple)
![YOLOv8](https://img.shields.io/badge/model-YOLOv8-teal)
![YOLOv9](https://img.shields.io/badge/model-YOLOv9-orange)
![All](https://img.shields.io/badge/DevKit-All-green)

This project demonstrates **SiMa.ai’s edge inferencing capability** for multi-channel video processing. It contains a **Vision AI pipeline** for processing up to 16 video channels on SiMa.ai’s **Modalix** or **MLSoC** (code name **Davinci**) platforms, requiring **SDK 1.7.0**.

## Overview

### Purpose
This pipeline processes up to 16 video channels in parallel, enabling real-time inference on edge devices.  

### Design
For **object detection** workloads, the pipeline includes a custom plugin called **OpenLabelConverter**, which generates metadata in the Open Label format and passes it to **OptiView** for rendering.

   **Preproc → MLA → BoxDecoder → OpenLabelConverter**

For **pose detection**, the MLA directly creates the overlay video output without using OpenLabelConverter.

### Channel → Model Mapping

| Channels   | Model   | Task              |
|------------|---------|-------------------|
| 1–4        | YOLOv9  | Object Detection  |
| 5–8        | YOLOv8  | Pose Estimation   |
| 9–12       | YOLOv8  | Object Detection  |
| 13–16      | YOLOv7  | Object Detection  |


## Prerequisites
- **SDK**: [Palette SDK 1.7.0](https://docs.sima.ai/pages/palette/installation.html) for building and deploying the Vision AI pipeline. 
- **sima-cli**: Required for downloading assets and managing SiMa.ai resources. See the [sima-cli documentation](https://docs.sima.ai/pages/sima_cli/main.html) for installation and login instructions.  
- **Hardware**: SiMa.ai DevKit with Modalix or MLSoC (Davinci).  

## Setup RTSP Sources

To test this sample, you will need to download mediasrc tool and video files to stream RTSP from your **host machine**. 

> [!IMPORTANT]
> Run the following command on your Linux or Mac host machine

```bash
mkdir multisrc && cd multisrc && sima-cli install assets/multi-video-sources
cd multi-video-sources-scripts/
open preview.html
./mediasrc.sh ../videos-480p30
```

On Windows, use `mediasrc.bat` instead.


## Setup Instructions

> [!IMPORTANT]
> CLI commands below are executed from from **within the Palette SDK**.
> Make sure to update `sima-cli` first to the latest version.

```bash
sima-cli install gh:sima-ai/pipeline-multichannel
```

This command pulls the source code and models, and prepares the folder for building.  

If you have already set up the RTSP sources, enter your host IP address when prompted.  

During installation, the process will update `application.json` to configure the pipeline to read from the correct source.


## Build The Project

> [!NOTE]
> Use `--board-type` according to your DevKit type.

```bash
mpk create -s . -d . --clean --board-type modalix
mpk create -s . -d . --clean --board-type davinci
```

## Deploy The Package

```bash
mpk device connect -t sima@devkit-ip
mpk deploy -f pipeline-multichannel.mpk
```

## Setup OptiView

OptiView is a tool that streamlines Vision AI development and evaluation.  

Follow the [installation guide](https://docs.sima.ai/pages/optiview/install.html#devkit) to set up OptiView on your DevKit and learn how to use it.

## Run the Pipeline

1. Open the OptiView web interface in your browser.  
2. From the dropdown menu, select **`pipeline-multichannel`**.  
4. Click the **Rocket** icon to start the pipeline.  
5. Click the **TV** icon in the bottom-left corner to switch to the live viewer and see the results.  