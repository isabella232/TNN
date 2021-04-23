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

#include <cmath>
#include <memory>

#include <inference_engine.hpp>
#include <ngraph/ngraph.hpp>
#include <ngraph/node.hpp>
#include <ngraph/op/op.hpp>
#include <ngraph/opsets/opset.hpp>
#include <ngraph/opsets/opset1.hpp>

#include "tnn/extern_wrapper/foreign_blob.h"
#include "tnn/extern_wrapper/foreign_tensor.h"
#include "tnn/layer/base_layer.h"
#include "tnn/network/openvino/layer_builder/openvino_layer_builder.h"
#include "tnn/network/openvino/openvino_types.h"

namespace TNN_NS {

DECLARE_OPENVINO_LAYER_BUILDER(OneHot, LAYER_ONEHOT);

Status OneHotOVLayerBuilder::Build() {
    auto paramlist = dynamic_cast<OneHotLayerParam*>(param_);

    if (GetInputNodes().size() <= 0) {
        LOGE("Error: 0 input nodes\n");
        return TNNERR_INIT_LAYER;
    }

    auto input_node = GetInputNodes();

    auto depthNode = std::make_shared<ngraph::op::Constant>(ngraph::element::i32, ngraph::Shape{}, paramlist->depth);
    auto onNode    = std::make_shared<ngraph::op::Constant>(ngraph::element::f32, ngraph::Shape{}, paramlist->value_on);
    auto offNode = std::make_shared<ngraph::op::Constant>(ngraph::element::f32, ngraph::Shape{}, paramlist->value_off);

    auto onehotNode = std::make_shared<ngraph::op::v1::OneHot>(input_node[0]->output(0), depthNode->output(0),
                                                               onNode->output(0), offNode->output(0), paramlist->axis);

    onehotNode->validate_and_infer_types();
    onehotNode->set_friendly_name(paramlist->name);

    ngraph::NodeVector outputNodes;
    outputNodes.push_back(onehotNode);
    SetOutputTensors(outputNodes);

    return TNN_OK;
}

REGISTER_OPENVINO_LAYER_BUILDER(OneHot, LAYER_ONEHOT);
}  // namespace TNN_NS