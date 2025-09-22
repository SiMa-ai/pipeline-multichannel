#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# -----------------------------------------------------------------------------
#
# This script is executed by sima-cli and prepares the multichannel project by:
#
#   1. Copying plugin sources from extracted ELF packages (Modalix and Davinci)
#      into the project's `plugins/` folder.
#   2. Cleaning up temporary ELF extraction directories and tarballs.
#   3. Asking the user for the host IP address that will be used for RTSP streaming.
#   4. Updating `application.json` to replace the default `127.0.0.1:8554`
#      with the user-provided host IP, keeping the port fixed at 8554.
#
# -----------------------------------------------------------------------------

set -e

# Step 1: Copy plugins from Modalix and Davinci ELF extractions
cp -r ../modalix_elf/plugins/* plugins/
cp -r ../davinci_elf/plugins/* plugins/

# Step 2: Cleanup extracted directories and tarballs
rm -rf ../modalix_elf ../davinci_elf ../*.tar.gz

# Step 3: Ask for host IP interactively
read -rp "Enter the host IP address for RTSP streaming: " HOST_IP

# Step 4: Update application.json (replace default localhost:8554 with user IP)
sed -i "s|127\.0\.0\.1:8554|${HOST_IP}:8554|g" application.json

# Step 5: Confirmation and next steps
echo "✅ Plugins copied and application.json updated with RTSP host ${HOST_IP}:8554"
