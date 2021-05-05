﻿/*
 *  This file is part of VSP.NET.
 *  VSP.NET is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VSP.NET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VSP.NET.  If not, see <https://www.gnu.org/licenses/>. *
 */

using System;
using System.Collections.Generic;

namespace CSGONET.API.Modules.Memory.Constants
{
    public enum DataType
    {
		DATA_TYPE_VOID,
        DATA_TYPE_BOOL,
        DATA_TYPE_CHAR,
        DATA_TYPE_UCHAR,
        DATA_TYPE_SHORT,
        DATA_TYPE_USHORT,
        DATA_TYPE_INT,
        DATA_TYPE_UINT,
        DATA_TYPE_LONG,
        DATA_TYPE_ULONG,
        DATA_TYPE_LONG_LONG,
        DATA_TYPE_ULONG_LONG,
        DATA_TYPE_FLOAT,
        DATA_TYPE_DOUBLE,
        DATA_TYPE_POINTER,
        DATA_TYPE_STRING,
        DATA_TYPE_VARIANT
    }

    public static class DataTypeExtensions
    {
        private static Dictionary<Type, DataType> types = new Dictionary<Type, DataType>()
        {
            {typeof(float), DataType.DATA_TYPE_FLOAT},
            {typeof(IntPtr), DataType.DATA_TYPE_POINTER},
            {typeof(int), DataType.DATA_TYPE_INT},
            {typeof(bool), DataType.DATA_TYPE_BOOL},
            {typeof(string), DataType.DATA_TYPE_STRING},
            {typeof(ulong), DataType.DATA_TYPE_ULONG },
            {typeof(short), DataType.DATA_TYPE_VARIANT }
        };

        public static DataType? ToDataType(this Type type)
        {
            if (types.ContainsKey(type)) return types[type];

            if (typeof(NativeObject).IsAssignableFrom(type))
            {
                return DataType.DATA_TYPE_POINTER;
            }

            return null;
        }
    }
}