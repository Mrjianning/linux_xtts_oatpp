# 使用官方的Ubuntu基础镜像
FROM ubuntu:20.04

# 设置时区和非交互安装模式
ENV TZ=Asia/Shanghai
ENV DEBIAN_FRONTEND=noninteractive

ENV LD_LIBRARY_PATH=/workspace:$LD_LIBRARY_PATH

# 更新包列表并安装必要的软件包
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    libssl-dev \
    libspdlog-dev \
    libpthread-stubs0-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# 创建工作目录
WORKDIR /workspace

# 复制项目文件到工作目录
COPY ./workspace /workspace

# 运行生成的可执行文件
CMD ["/workspace/xtts_server"]
