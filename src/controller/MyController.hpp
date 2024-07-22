#ifndef MyController_hpp
#define MyController_hpp

#include "dto/MyDTOs.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "xtts/XTTSClient.hpp"

#include <spdlog/spdlog.h>

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- 开始代码生成

/**
 * 示例 ApiController
 * 创建 ENDPOINT 的基本示例
 * 更多详情请访问 oatpp.io
 */
class MyController : public oatpp::web::server::api::ApiController {
protected:
  MyController(const std::shared_ptr<ObjectMapper>& objectMapper)
  : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:
  
  /**
   * 在这里注入 @objectMapper 组件作为默认参数
   * 不要返回裸的 Controllable* 对象！请使用 shared_ptr！
   */
  static std::shared_ptr<MyController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>,
                                                                    objectMapper)){
    return std::shared_ptr<MyController>(new MyController(objectMapper));
  }
  
  /**
   * Hello World 端点 Coroutine 映射到 "/" (根路径)
   */
  ENDPOINT_ASYNC("GET", "/", Root) {
    
    ENDPOINT_ASYNC_INIT(Root)
    
    /**
     * Coroutine 入口点 act()
     * 返回 Action (下一步要做什么)
     */
    Action act() override {
      auto dto = HelloDto::createShared();
      dto->message = "Hello Async!";
      dto->server = Header::Value::SERVER;
      dto->userAgent = request->getHeader(Header::USER_AGENT);
      return _return(controller->createDtoResponse(Status::CODE_200, dto));
    }
    
  };
  
  /**
   * Echo body 端点 Coroutine。映射到 "/body/string"。
   * 返回请求中接收到的 body
   */
  ENDPOINT_ASYNC("POST", "/body/string", EchoStringBody) {
    
    ENDPOINT_ASYNC_INIT(EchoStringBody)
    
    Action act() override {
      /* 返回 Action 以启动子协程读取 body */
      return request->readBodyToStringAsync().callbackTo(&EchoStringBody::returnResponse);
    }
    
    Action returnResponse(const oatpp::String& body){
      /* 返回 Action 以返回创建的 OutgoingResponse */
      return _return(controller->createResponse(Status::CODE_200, body));
    }
    
  };
  
  /**
   * Echo body 端点 Coroutine。映射到 "/body/dto"。
   * 反序列化接收到的 DTO，并返回相同的 DTO
   * 返回请求中接收到的 MessageDto 类型的 body
   */
  ENDPOINT_ASYNC("POST", "/xtts", EchoDtoBody) {
    
    ENDPOINT_ASYNC_INIT(EchoDtoBody)
    
    Action act() override {
      return request->readBodyToDtoAsync<oatpp::Object<XTTSDto>>(controller->getDefaultObjectMapper()).callbackTo(&EchoDtoBody::returnResponse);
    }
    
    Action returnResponse(const oatpp::Object<XTTSDto>& body){

      // 接受配置
      nlohmann::json config;
      config["vcn"] = body->vcn ? body->vcn->c_str() : nullptr;
      config["vcnModel"] = body->vcnModel ? body->vcnModel->c_str() : nullptr;
      config["language"] = static_cast<int>(body->language); // 转换为 int
      config["speed"] = static_cast<int>(body->speed); // 转换为 int
      config["pitch"] = static_cast<int>(body->pitch); // 转换为 int
      config["volume"] = static_cast<int>(body->volume); // 转换为 int
      config["text"] = body->text ? body->text->c_str() : nullptr;
      config["sample_rate"] = static_cast<int>(body->sample_rate); // 转换为 int
      config["bits_per_sample"] = static_cast<int>(body->bits_per_sample); // 转换为 int
      config["channels"] = static_cast<int>(body->channels); // 转换为 int
      config["textEncoding"] = body->textEncoding ? body->textEncoding->c_str() : nullptr;
      config["audio_name"]=config["text"].get<std::string>();

      XTTSClient xtts_client;
      xtts_client.createXTTS(config);

      // 获取语音数据
      std::string audio_name = "./audio/" + config["text"].get<std::string>() + ".wav";
      std::vector<char> audioData=xtts_client.readLocalWavFile(audio_name);
      
      // 打印语音数据大小
      std::cout << "======================================================> 语音数据大小: " << audioData.size() << " bytes" << std::endl;

      // 创建响应
      auto response = controller->createResponse(Status::CODE_200, oatpp::String((const char*)audioData.data(), audioData.size()));
      response->putHeader(Header::CONTENT_TYPE, "audio/wav");
      return _return(response);
    }
    
  };
  
};

#include OATPP_CODEGEN_END(ApiController) //<-- 结束代码生成

#endif /* MyController_hpp */
