
#ifndef MyDTOs_hpp
#define MyDTOs_hpp

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */
class HelloDto : public oatpp::DTO {
  
  DTO_INIT(HelloDto, DTO)
  
  DTO_FIELD(String, userAgent, "user-agent");
  DTO_FIELD(String, message);
  DTO_FIELD(String, server);
  
};

class MessageDto : public oatpp::DTO {
  
  DTO_INIT(MessageDto, DTO)
  
  DTO_FIELD(String, message);
  
};

class XTTSDto : public oatpp::DTO {

  DTO_INIT(XTTSDto, DTO)
  DTO_FIELD(String, vcn);
  DTO_FIELD(String, vcnModel);
  DTO_FIELD(Int32, language);
  DTO_FIELD(Int32, speed);
  DTO_FIELD(Int32, pitch);
  DTO_FIELD(Int32, volume);
  DTO_FIELD(String, text);
  DTO_FIELD(Int32, sample_rate);
  DTO_FIELD(Int32, bits_per_sample);
  DTO_FIELD(Int32, channels);
  DTO_FIELD(String, textEncoding);
  
};


#include OATPP_CODEGEN_END(DTO)

#endif /* MyDTOs_hpp */
