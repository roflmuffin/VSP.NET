﻿using System;
using CSGONET.API.Core;
using CSGONET.API.Modules.Utils;

namespace CSGONET.API.Modules.Engine.Trace
{
    public class TraceEngine
    {
        public static IntPtr CreateRay(RayType rayType, Vector vec1, Vector vec2)
        {
            return NativeAPI.CreateRay1((int) rayType, vec1.Handle, vec2.Handle);
        }

        public static IntPtr CreateRay(Vector vec1, Vector vec2, Vector vec3, Vector vec4)
        {
            return NativeAPI.CreateRay2(vec1.Handle, vec2.Handle, vec3.Handle, vec4.Handle);
        }

        public static TraceResult TraceRay(IntPtr ray, uint mask, ITraceFilter filter)
        {
            var tr = new TraceResult();
            var proxy = new TraceFilterProxy(filter);
            NativeAPI.TraceRay(ray, tr.Handle, proxy.Handle, mask);
            return tr;
        }
    }
}