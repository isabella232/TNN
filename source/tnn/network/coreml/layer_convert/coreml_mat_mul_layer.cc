// Tencent is pleased to support the open source community by making TNN available.
//
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include "coreml_base_layer.h"
#include "tnn/utils/naive_compute.h"

namespace TNN_NS {

DECLARE_COREML_LAYER_WITH_DATA(MatMul, LAYER_MATMUL,
                                    std::shared_ptr<CoreML__Specification__WeightParams> weight_param_;
                                    std::shared_ptr<RawBuffer> rawbuffer_fp32_weight_;);

Status CoreMLMatMulLayer::BuildLayerType() {
    //layer type
    coreml_layer_->layer_case = CORE_ML__SPECIFICATION__NEURAL_NETWORK_LAYER__LAYER_BATCHED_MATMUL;
    return TNN_OK;
}

Status CoreMLMatMulLayer::BuildLayerParam() {
    //layer param
    auto param = dynamic_cast<MatMulLayerParam *>(layer_info_->param.get());
    auto resource = dynamic_cast<MatMulLayerResource *>(layer_resource_);
    if (!param || (layer_info_->inputs.size() == 1 && !resource)) {
        return Status(TNNERR_MODEL_ERR, "CoreMLMatMulLayer has invalid MatMulLayerParam or MatMulLayerResource");
    }
    
    auto matrix_a_dims = param->matrix_a_dims;
    auto matrix_b_dims = param->matrix_b_dims;
    
    coreml_layer_param_ = std::shared_ptr<CoreML__Specification__BatchedMatMulLayerParams>(new CoreML__Specification__BatchedMatMulLayerParams);
    coreml_layer_->batchedmatmul = (CoreML__Specification__BatchedMatMulLayerParams *)coreml_layer_param_.get();
    core_ml__specification__batched_mat_mul_layer_params__init(coreml_layer_->batchedmatmul);
    coreml_layer_->batchedmatmul->weightmatrixfirstdimension = matrix_a_dims.back();
    coreml_layer_->batchedmatmul->weightmatrixseconddimension = matrix_b_dims.back();
    coreml_layer_->batchedmatmul->hasbias = false;
    
    if (layer_info_->inputs.size() == 1) {
        //CoreML only support matrix b with dim size = 2, and matrix b is stored in column order
        if (matrix_b_dims.size() != 2) {
            return Status(TNNERR_MODEL_ERR, "CoreMLMatMulLayer has invalid matrix_b_dims");
        }
        
        if (param->weight_position == 0) {
            return Status(TNNERR_MODEL_ERR, "CoreMLMatMulLayer dont support constant matrix a");
        } else {
            RETURN_ON_NEQ(RawBuffer2CoreMLWeight(&(resource->weight),weight_param_, rawbuffer_fp32_weight_), TNN_OK);
            coreml_layer_->batchedmatmul->weights = weight_param_.get();
        }
    }
    
    return TNN_OK;
}

Status CoreMLMatMulLayer::BuildConstantWeightsLayer() {
    return CoreMLBaseLayer::BuildConstantWeightsLayer();
}

std::vector<std::string> CoreMLMatMulLayer::BuildLayerInputs() {
    return CoreMLBaseLayer::BuildLayerInputs();
}

std::vector<std::string> CoreMLMatMulLayer::BuildLayerOutputs() {
    return CoreMLBaseLayer::BuildLayerOutputs();
}

REGISTER_COREML_LAYER(MatMul, LAYER_MATMUL);

}  // namespace TNN_NS
