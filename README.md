# Đây là project Game Engine Vulkan

## Hướng Dẫn Cài Đặt & Chạy Dự Án

### 1. Clone project về máy
```bash
git clone https://github.com/EsercitoHaki/zephyr.git
cd zephyr
```

### 2. Cài đặt Cmake
Trên Window
```bash
winget install Kitware.CMake
```
Trên Linux (Distro Arach)
```bash
sudo pacman -Syu
sudo pacman -S cmake
```
Trên macOS
```bash
brew install cmake
```

### 3. Cài đặt Vulkan SDK
```bash
./tools/install-vulkan-sdk.sh
```

### 4. Build
```bash
mkdir build
cd build
cmake ..
make
```

### 5. Run
```bash
./engine/src/game
```

Các bước thực hiện:
- Khởi tạo Vulkan (sử dụng vk-boootstrap)
    - Tạo Vulkan Instance
    - Chọn Physical Device
    - Tạo Logical Device
    - Tạo Surface và Swapchain
    - Tạo VmaAllocator

- Quản lý Command Buffers và đồng bộ hóa
    - Tạo Command Pool cho GPU queue
    - Cấp phát Command Buffers để ghi lệnh

- Thiết lập Synchronization (Semaphores, Fences)
    - Tạo Fences để đồng bộ hóa CPU-GPU
    - Tạo Semaphores để đồng bộ hóa GPU-GPU

- Tạo Render Pass và Framebuffer
    - Xác định các Attachment cho Render Pass
    - Tạo Framebuffer cho từng ảnh trong Swapchain

- Tạo Pipeline và Shader Modules
    - Tạo Shader Module từ tệp .spv
    - Thiết lập VkPipeline để vẽ hình ảnh

- Ghi lệnh vẽ (Record Command Buffers)
    - Bắt đầu ghi Command Buffer
    - Gửi lệnh vẽ vào Render Pass
    - Kết thúc ghi Command Buffer

- Gửi Command Buffer đến GPU và trình bày hình ảnh
    - Gửi Command Buffer đến hàng đợi đồ họa (vkQueueSubmit)
    - Trình bày ảnh lên màn hình (vkQueuePresentKHR)

## Model Loadding

- Sử dụng GLFW thay vì dùng SDL, để có thể tiện xử lý các model 3D

Một số tài liệu liên quan tới đồ họa máy tính:
- Ray tracing in one weekend
- Physically Based Rendering (PBR)
