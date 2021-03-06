// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include "olap/field.h"
#include "exprs/aggregate_functions.h"

using std::map;
using std::nothrow;
using std::string;

namespace doris {

Field* Field::create(const FieldInfo& field_info) {
    Field* field = new Field(field_info);
    return field;
}

// 这个函数目前不支持字符串类型
Field* Field::create_by_type(const FieldType& type) {
    Field* field = NULL;
    FieldInfo field_info;
    field_info.aggregation = OLAP_FIELD_AGGREGATION_NONE;
    field_info.type = type;

    if (type == OLAP_FIELD_TYPE_CHAR || type == OLAP_FIELD_TYPE_VARCHAR
            || type == OLAP_FIELD_TYPE_HLL) {
        field = NULL;
    } else {
        field = new Field(field_info);
    }

    return field;
}

Field::Field(const FieldInfo& field_info)
        : _type(field_info.type),
          _index_size(field_info.index_length),
          _offset(0) {

    _type_info = get_type_info(field_info.type);
    if (_type == OLAP_FIELD_TYPE_CHAR || _type == OLAP_FIELD_TYPE_VARCHAR
            || _type == OLAP_FIELD_TYPE_HLL) {
        _size = sizeof(StringSlice);
    } else {
        /*
         * the field_info.size and field_info.index_length is equal to zero,
         * if field_info is generated by Field::create_by_type function.
         * ColumnStatistics use size but not index_size.
         */
        _size = _type_info->size();
    }
    _index_size = field_info.index_length;
    _aggregate_func = get_aggregate_func(field_info.aggregation, field_info.type);
    _finalize_func = get_finalize_func(field_info.aggregation, field_info.type);
}

}  // namespace doris
