// Copyright (C) 2019-2022 Intel Corporation
// Licensed under the MIT License

#pragma once

#include <vector>

#include <inference_engine.hpp>
#if defined (OPENVINO_2022_1) || (OPENVINO_2022_2) || (OPENVINO_2022_3)
#define OV_API_20
#include "openvino/openvino.hpp"
#include "openvino/pass/convert_fp32_to_fp16.hpp"
#endif

#ifdef IO_BUFFER_ENABLED
#include <gpu/gpu_context_api_ocl.hpp>
#include <gpu/gpu_config.hpp>
#include <openvino/runtime/intel_gpu/ocl/ocl.hpp>
#endif

#include <string>

namespace onnxruntime {
namespace openvino_ep {
class OVCore;
class OVInferRequest;
class OVExeNetwork;

typedef InferenceEngine::Precision OVPrecision;
typedef ov::Tensor OVTensor;
typedef ov::ProfilingInfo OVProfilingInfo;
typedef ov::AnyMap OVConfig;
typedef ov::Model OVNetwork;
typedef std::shared_ptr<OVInferRequest> OVInferRequestPtr;
typedef std::shared_ptr<OVTensor> OVTensorPtr;

#ifdef IO_BUFFER_ENABLED
  typedef ov::intel_gpu::ocl::ClContext* OVRemoteContextPtr;
  typedef ov::RemoteContext OVRemoteContext;
#endif

  class OVCore {
    ov::Core oe;
    public:
        std::shared_ptr<OVNetwork> ReadModel(const std::string& model_stream) const;
        OVExeNetwork LoadNetwork(std::shared_ptr<OVNetwork>& ie_cnn_network, std::string& hw_target, OVConfig& config, ov::AnyMap& device_config, std::string name);
        void SetCache(std::string cache_dir_path);
        #ifdef IO_BUFFER_ENABLED
        OVExeNetwork LoadNetwork(std::shared_ptr<OVNetwork>& model, OVRemoteContextPtr context, std::string& name);
        #endif
        std::vector<std::string> GetAvailableDevices();
        ov::Core& Get() {
            return oe;
        }
    };

    class OVExeNetwork {
      ov::CompiledModel obj;
    public:
        OVExeNetwork(ov::CompiledModel md) { obj = md; }
        OVExeNetwork() { obj = ov::CompiledModel(); }
        ov::CompiledModel& Get() { return obj; }
        OVInferRequest CreateInferRequest();
    };

    class OVInferRequest {
        ov::InferRequest ovInfReq;
    public:
        OVTensorPtr GetTensor(const std::string& name);
        void SetTensor(const std::string& name, OVTensorPtr& blob);
        void StartAsync();
        void WaitRequest();
        void QueryStatus();
        explicit OVInferRequest(ov::InferRequest obj) { ovInfReq = obj; }
        OVInferRequest() { ovInfReq = ov::InferRequest(); }
        ov::InferRequest& GetNewObj() {
        return ovInfReq;
        }
    };
   }
}  // namespace onnxruntime
