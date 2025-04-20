#!/bin/bash
set -e

# Biến cấu hình
FORCE=0
CHECK_ONLY=0
VERBOSE=0

# Xử lý tham số dòng lệnh
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --force) FORCE=1 ;;
        --check-only) CHECK_ONLY=1 ;;
        --verbose) VERBOSE=1 ;;
        --help)
            echo "Cài đặt Vulkan SDK"
            echo "Tùy chọn:"
            echo "  --force      : Bỏ qua kiểm tra và cài đặt lại"
            echo "  --check-only : Chỉ kiểm tra cài đặt, không cài đặt"
            echo "  --verbose    : Hiển thị thông tin chi tiết hơn"
            echo "  --help       : Hiển thị trợ giúp này"
            exit 0
            ;;
        *) echo "Tùy chọn không hợp lệ: $1. Sử dụng --help để xem hướng dẫn."; exit 1 ;;
    esac
    shift
done

# Hàm tiện ích
log() {
    echo -e "\033[1;34m[INFO]\033[0m $1"
}

warn() {
    echo -e "\033[1;33m[WARNING]\033[0m $1" >&2
}

err() {
    echo -e "\033[1;31m[ERROR]\033[0m $1" >&2
    exit 1
}

verbose() {
    if [[ "$VERBOSE" -eq 1 ]]; then
        echo -e "\033[0;36m[VERBOSE]\033[0m $1"
    fi
}

# Phát hiện loại distro Linux
detect_distro() {
    if [[ -f /etc/os-release ]]; then
        source /etc/os-release
        case "$ID" in
            debian|ubuntu|linuxmint|pop|elementary|zorin)
                echo "debian"
                ;;
            fedora|centos|rhel|rocky|almalinux)
                echo "fedora"
                ;;
            opensuse*|suse*)
                echo "suse"
                ;;
            arch|manjaro|endeavouros|garuda)
                echo "arch"
                ;;
            void)
                echo "void"
                ;;
            *)
                echo "unknown"
                ;;
        esac
    elif [[ -f /etc/redhat-release ]]; then
        echo "fedora"
    elif [[ -f /etc/arch-release ]]; then
        echo "arch"
    elif [[ -f /etc/debian_version ]]; then
        echo "debian"
    else
        echo "unknown"
    fi
}

# Kiểm tra Vulkan SDK
check_vulkan() {
    log "Đang kiểm tra Vulkan SDK..."

    if command -v vulkaninfo >/dev/null 2>&1; then
        log "Vulkan SDK đã được cài đặt."

        # Thông tin cơ bản
        log "Thông tin GPU:"
        if [[ "$VERBOSE" -eq 1 ]]; then
            vulkaninfo --summary 2>/dev/null || vulkaninfo | grep -iE 'GPU|deviceName|driverVersion'
        else
            vulkaninfo --summary 2>/dev/null || vulkaninfo | grep -iE 'GPU|deviceName' | head -n 3
        fi

        # Kiểm tra biến môi trường
        if [[ -n "$VULKAN_SDK" ]]; then
            log "Biến môi trường VULKAN_SDK đã được thiết lập: $VULKAN_SDK"
        else
            warn "Biến môi trường VULKAN_SDK chưa được thiết lập"
        fi

        return 0
    else
        if [[ "$CHECK_ONLY" -eq 1 ]]; then
            err "Vulkan SDK chưa được cài đặt."
        else
            warn "Vulkan SDK chưa được cài đặt. Sẽ tiến hành cài đặt..."
            return 1
        fi
    fi
}

# Phát hiện và cài đặt driver GPU
install_gpu_driver() {
    log "Đang phát hiện GPU..."
    local gpu=$(lspci | grep -i 'vga')
    local distro=$1

    if echo "$gpu" | grep -iq 'amd'; then
        log "Phát hiện GPU AMD. Cài đặt driver..."
        case "$distro" in
            debian)
                sudo apt install -y mesa-vulkan-drivers libvulkan1 vulkan-tools
                ;;
            fedora)
                sudo dnf install -y mesa-vulkan-drivers vulkan-loader
                ;;
            arch)
                sudo pacman -S --noconfirm vulkan-radeon lib32-vulkan-radeon
                ;;
            suse)
                sudo zypper install -y Mesa-vulkan-drivers
                ;;
            void)
                sudo xbps-install -y mesa-vulkan-drivers
                ;;
        esac
    elif echo "$gpu" | grep -iq 'intel'; then
        log "Phát hiện GPU Intel. Cài đặt driver..."
        case "$distro" in
            debian)
                sudo apt install -y mesa-vulkan-drivers libvulkan1 vulkan-tools
                ;;
            fedora)
                sudo dnf install -y mesa-vulkan-drivers vulkan-loader
                ;;
            arch)
                sudo pacman -S --noconfirm vulkan-intel lib32-vulkan-intel
                ;;
            suse)
                sudo zypper install -y Mesa-vulkan-drivers
                ;;
            void)
                sudo xbps-install -y mesa-vulkan-drivers
                ;;
        esac
    elif echo "$gpu" | grep -iq 'nvidia'; then
        log "Phát hiện GPU NVIDIA. Cài đặt driver..."
        case "$distro" in
            debian)
                sudo apt install -y nvidia-driver-* nvidia-vulkan-icd libvulkan1 vulkan-tools
                ;;
            fedora)
                sudo dnf install -y xorg-x11-drv-nvidia-cuda vulkan-loader
                ;;
            arch)
                sudo pacman -S --noconfirm nvidia nvidia-utils lib32-nvidia-utils
                ;;
            suse)
                sudo zypper install -y nvidia-vulkan-driver
                ;;
            void)
                sudo xbps-install -y nvidia
                ;;
        esac
    else
        warn "Không phát hiện được GPU cụ thể. Bỏ qua cài đặt driver chuyên biệt."
        # Cài đặt driver generic
        case "$distro" in
            debian)
                sudo apt install -y libvulkan1 vulkan-tools
                ;;
            fedora)
                sudo dnf install -y vulkan-loader vulkan-tools
                ;;
            arch)
                sudo pacman -S --noconfirm vulkan-icd-loader vulkan-tools
                ;;
            suse)
                sudo zypper install -y vulkan
                ;;
            void)
                sudo xbps-install -y vulkan-loader vulkan-tools
                ;;
        esac
    fi
}

# Cài đặt Vulkan trên các distro Linux
install_vulkan_debian() {
    log "Cài đặt Vulkan SDK trên Debian/Ubuntu..."

    # Thêm repo LunarG (nếu cần)
    if [[ "$FORCE" == "1" ]]; then
        wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo gpg --dearmor -o /etc/apt/trusted.gpg.d/lunarg.gpg

        # Phát hiện phiên bản Ubuntu
        local ubuntu_version="jammy"  # mặc định
        if grep -q "22.04" /etc/os-release; then
            ubuntu_version="jammy"
        elif grep -q "20.04" /etc/os-release; then
            ubuntu_version="focal"
        elif grep -q "24.04" /etc/os-release; then
            ubuntu_version="noble"
        fi

        sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-${ubuntu_version}.list https://packages.lunarg.com/vulkan/lunarg-vulkan-${ubuntu_version}.list
    fi

    sudo apt update
    sudo apt install -y vulkan-tools libvulkan-dev vulkan-validationlayers-dev

    # Cài đặt driver GPU
    install_gpu_driver "debian"
}

install_vulkan_fedora() {
    log "Cài đặt Vulkan SDK trên Fedora/RHEL..."
    sudo dnf update -y
    sudo dnf install -y vulkan-tools vulkan-loader-devel vulkan-validation-layers-devel

    # Cài đặt driver GPU
    install_gpu_driver "fedora"
}

install_vulkan_arch() {
    log "Cài đặt Vulkan SDK trên Arch Linux..."
    sudo pacman -Syu --noconfirm
    sudo pacman -S --noconfirm vulkan-tools vulkan-headers vulkan-icd-loader vulkan-validation-layers

    # Cài đặt driver GPU
    install_gpu_driver "arch"
}

install_vulkan_suse() {
    log "Cài đặt Vulkan SDK trên openSUSE..."
    sudo zypper refresh
    sudo zypper install -y vulkan-tools vulkan-devel vulkan-validationlayers

    # Cài đặt driver GPU
    install_gpu_driver "suse"
}

install_vulkan_void() {
    log "Cài đặt Vulkan SDK trên Void Linux..."
    sudo xbps-install -Su
    sudo xbps-install -y vulkan-tools vulkan-loader vulkan-validation-layers

    # Cài đặt driver GPU
    install_gpu_driver "void"
}

# Tạo và chạy script PowerShell trên Windows
install_vulkan_windows() {
    log "Tạo script PowerShell cho Windows..."
    cat <<'EOF' > install_vulkan_windows.ps1
$ErrorActionPreference = "Stop"

function Log {
    param([string]$message)
    Write-Host "[INFO] $message" -ForegroundColor Blue
}

function Warn {
    param([string]$message)
    Write-Host "[WARNING] $message" -ForegroundColor Yellow
}

function Error {
    param([string]$message)
    Write-Host "[ERROR] $message" -ForegroundColor Red
    exit 1
}

# Kiểm tra xem Vulkan SDK đã được cài đặt chưa
function Check-VulkanSDK {
    $vulkanRegistry = Get-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\*" -ErrorAction SilentlyContinue |
                       Where-Object { $_.DisplayName -like "*Vulkan SDK*" } |
                       Select-Object -First 1

    if ($vulkanRegistry) {
        Log "Phát hiện Vulkan SDK đã cài đặt: $($vulkanRegistry.DisplayName)"
        $sdkPath = $vulkanRegistry.InstallLocation
        return $sdkPath
    }

    return $null
}

# Tải và cài đặt Vulkan SDK
function Install-VulkanSDK {
    $tempDir = Join-Path $env:TEMP "VulkanSDK"
    New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
    $installerPath = Join-Path $tempDir "vulkan-sdk.exe"

    try {
        Log "Đang tải Vulkan SDK từ LunarG..."
        $progressPreference = 'SilentlyContinue'
        Invoke-WebRequest -Uri "https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe" -OutFile $installerPath
        $progressPreference = 'Continue'

        Log "Đang cài đặt Vulkan SDK..."
        Start-Process -FilePath $installerPath -ArgumentList "/S" -Wait

        # Kiểm tra cài đặt
        $sdkPath = Check-VulkanSDK
        if ($sdkPath) {
            Log "Vulkan SDK đã được cài đặt thành công tại: $sdkPath"
            return $sdkPath
        } else {
            Error "Không thể xác minh cài đặt Vulkan SDK. Vui lòng cài đặt thủ công."
        }
    }
    catch {
        Error "Lỗi khi cài đặt Vulkan SDK: $_"
    }
}

# Thiết lập biến môi trường
function Setup-VulkanEnvironment {
    param([string]$sdkPath)

    # Thiết lập VULKAN_SDK
    [System.Environment]::SetEnvironmentVariable("VULKAN_SDK", $sdkPath, "Machine")

    # Thêm vào PATH nếu cần
    $envPath = [System.Environment]::GetEnvironmentVariable("Path", "Machine")
    $binPath = Join-Path $sdkPath "Bin"

    if ($envPath -notlike "*$binPath*") {
        $newPath = "$envPath;$binPath"
        [System.Environment]::SetEnvironmentVariable("Path", $newPath, "Machine")
        Log "Đã thêm $binPath vào PATH hệ thống"
    }
}

# Chương trình chính
Log "Kiểm tra cài đặt Vulkan SDK..."
$sdkPath = Check-VulkanSDK

if ($sdkPath) {
    $choice = Read-Host "Vulkan SDK đã được cài đặt. Bạn có muốn cài đặt lại không? (y/N)"
    if ($choice -eq "y" -or $choice -eq "Y") {
        $sdkPath = Install-VulkanSDK
    }
} else {
    Log "Chưa phát hiện Vulkan SDK. Bắt đầu cài đặt..."
    $sdkPath = Install-VulkanSDK
}

# Thiết lập biến môi trường
Setup-VulkanEnvironment $sdkPath

# Thông báo hoàn tất
Log "Cài đặt Vulkan SDK hoàn tất."
Log "Cần khởi động lại Windows để các thay đổi có hiệu lực."
EOF

    log "Đã tạo script PowerShell 'install_vulkan_windows.ps1'"

    # Kiểm tra môi trường
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        log "Đang chạy PowerShell script..."
        powershell.exe -ExecutionPolicy Bypass -File ./install_vulkan_windows.ps1
    else
        log "Để cài đặt trên Windows, chạy script 'install_vulkan_windows.ps1' bằng PowerShell"
    fi
}

# Cài đặt Vulkan trên macOS
install_vulkan_macos() {
    log "Kiểm tra Homebrew..."
    if ! command -v brew &> /dev/null; then
        warn "Homebrew chưa được cài đặt. Cài đặt Homebrew trước..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

        if ! command -v brew &> /dev/null; then
            err "Không thể cài đặt Homebrew. Vui lòng cài đặt thủ công."
        fi
    fi

    log "Cài đặt Vulkan SDK (MoltenVK) trên macOS..."
    brew update
    brew install molten-vk vulkan-headers vulkan-tools

    # Kiểm tra cài đặt
    if brew list | grep -q "molten-vk"; then
        log "MoltenVK đã được cài đặt thành công."
    else
        err "Không thể cài đặt MoltenVK."
    fi
}

# Hàm chính
run_main() {
    # Kiểm tra OS
    os_name=$(uname -s | tr '[:upper:]' '[:lower:]')
    verbose "Phát hiện OS: $os_name"

    # Chỉ kiểm tra nếu được yêu cầu
    if [[ "$CHECK_ONLY" == "1" ]]; then
        check_vulkan
        exit 0
    fi

    # Kiểm tra nếu Vulkan đã được cài đặt và không cần cài lại
    if [[ "$FORCE" == "0" ]] && check_vulkan 2>/dev/null; then
        log "Vulkan SDK đã được cài đặt. Sử dụng --force để cài đặt lại."
        exit 0
    fi

    # Cài đặt theo OS
    case "$os_name" in
        linux)
            # Kiểm tra quyền sudo
            if [[ "$FORCE" == "0" && "$(id -u)" != "0" && -z "$SUDO_USER" ]]; then
                warn "Khuyến nghị chạy script với quyền sudo."
                read -p "Tiếp tục? (y/N) " -n 1 -r
                echo
                if [[ ! $REPLY =~ ^[Yy]$ ]]; then
                    log "Đã hủy cài đặt."
                    exit 0
                fi
            fi

            # Phát hiện distro
            distro=$(detect_distro)
            verbose "Phát hiện distro Linux: $distro"

            # Cài đặt theo distro
            case "$distro" in
                debian)
                    install_vulkan_debian
                    ;;
                fedora)
                    install_vulkan_fedora
                    ;;
                arch)
                    install_vulkan_arch
                    ;;
                suse)
                    install_vulkan_suse
                    ;;
                void)
                    install_vulkan_void
                    ;;
                *)
                    err "Không hỗ trợ distro Linux này: $distro"
                    ;;
            esac
            ;;
        darwin)
            install_vulkan_macos
            ;;
        msys*|mingw*|cygwin*)
            install_vulkan_windows
            ;;
        *)
            err "Hệ điều hành không được hỗ trợ: $os_name"
            ;;
    esac

    # Kiểm tra cài đặt sau khi hoàn tất
    if [[ "$os_name" != "msys"* && "$os_name" != "mingw"* && "$os_name" != "cygwin"* ]]; then
        check_vulkan
    fi

    log "Cài đặt Vulkan SDK hoàn tất."
}

# Thực thi
run_main "$@"
