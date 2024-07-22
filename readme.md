### README

# 讯飞高品质版语音合成能力

## 项目简介

本项目实现了使用讯飞高品质版语音合成能力的示例。通过该示例，您可以将文本转换为高品质的语音输出，并保存为 WAV 文件，并封装接口进行调用。

## 运行教程

### 第一步：修改 `config.json` 中的 key

在运行项目之前，请确保在 `config/config.json` 文件中填入正确的 API Key 和其他配置信息。以下是 `config.json` 文件的示例格式：

```json
{
    "appID": "YOUR_APP_ID",
    "apiKey": "YOUR_API_KEY",
    "apiSecret": "YOUR_API_SECRET"
}
```

请将 `"YOUR_APP_ID"`、`"YOUR_API_KEY"` 和 `"YOUR_API_SECRET"` 替换为您从讯飞平台获得的实际值。

### 第二步：建立 build 目录并编译项目

1. 打开终端并导航到项目根目录。

2. 创建 `build` 目录：

   ```sh
   mkdir build
   ```

3. 进入 `build` 目录：

   ```sh
   cd build
   ```

4. 使用 CMake 配置项目：

   ```sh
   cmake ..
   ```

5. 编译项目：

   ```sh
   make
   ```

### 第三步：在 workspace 目录下运行可执行文件启动服务

1. 编译成功后，生成的可执行文件将位于 `workspace` 目录中。

2. 在 `workspace` 目录下运行：

   ```sh
   cd ../workspace
   ```

3. 启动服务：

   ```sh
   ./xtts_server
   ```

4. 请求接口

使用以下示例 JSON 请求接口：

```json
{
    "vcn": "xiaofeng",
    "vcnModel": "xiaofeng",
    "language": 1,
    "speed": 50,
    "pitch": 50,
    "volume": 100,
    "textEncoding": "UTF-8",
    "text": "测试测试接口",
    "sample_rate": 16000,
    "bits_per_sample": 16,
    "channels": 1
}
```

运行后，程序将读取 `config.json` 中的配置信息，执行语音合成，并生成 `output.wav` 文件返回。

### 第四步：使用 Docker 构建和运行

1. 构建 Docker 镜像：

   ```sh
   docker build -t aikit .
   ```

2. 运行 Docker 容器：

   ```sh
   docker run --name aikit_xtts -d -it -p 9999:9999 --restart=always aikit
   ```

3. 访问服务：

   在浏览器中打开 `http://localhost:9999` 或使用相应的客户端工具发送请求到服务端口。

## 目录结构

项目的目录结构如下：

```
├── CMakeLists.txt
├── README.md
├── src
│   ├── App.cpp
│   ├── controller
│   │   └── MyController.cpp
│   ├── xtts
│   │   ├── XTTSClient.cpp
│   │   └── XTTSClient.hpp
├── lib
│   ├── tts_libs
│   │   ├── include
│   │   └── lib
│   ├── install_json
│   │   ├── include
│   │   └── lib
│   ├── install_obb
│   │   └── lib
│   ├── oatpp_install
│       ├── include
│       └── lib
├── build
├── workspace
│   └──config
│       └── config.json
```

## 注意事项

- 请确保 `config/config.json` 文件存在且格式正确。
- 请根据您的环境调整 `CMakeLists.txt` 中的库路径。
- 在首次运行前，请确保已安装所有必要的依赖库。

## 支持

如有任何问题，请联系 [1046016768@qq.com]。

---

希望这个 README 文件对你有所帮助。如果你还有其他问题或需要进一步的调整，请随时告知。