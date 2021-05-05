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
using System.Collections.Concurrent;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace CSGONET.API.Core
{
	public class NativeException : Exception
	{
		public NativeException(string? message) : base(message)
		{
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	[Serializable]
	public unsafe struct fxScriptContext
	{
		public int numArguments;
		public int numResults;
		public int hasError;

		public ulong nativeIdentifier;
		public fixed byte functionData[8 * 32];
        public fixed byte result[8];
    }

	public class ScriptContext
	{
		[ThreadStatic]
		private static ScriptContext _globalScriptContext;

		public static ScriptContext GlobalScriptContext
		{
			get
			{
				if (_globalScriptContext == null) _globalScriptContext = new ScriptContext();
				return _globalScriptContext;
			}
		}

        public unsafe ScriptContext()
		{
            //Console.WriteLine("Global context address: " + (IntPtr)m_extContext);
		}

        public unsafe ScriptContext(fxScriptContext* context)
        {
            m_extContext = *context;
            //Console.WriteLine("Global context address: " + (IntPtr)m_extContext);
        }

		private readonly ConcurrentQueue<Action> ms_finalizers = new ConcurrentQueue<Action>();

		private readonly object ms_lock = new object();

		internal object Lock => ms_lock;

		internal fxScriptContext m_extContext = new fxScriptContext();

		[SecuritySafeCritical]
		public void Reset()
		{
			InternalReset();
		}

		[SecurityCritical]
		private void InternalReset()
		{
			m_extContext.numArguments = 0;
			m_extContext.numResults = 0;
            m_extContext.hasError = 0;
            //CleanUp();
        }

		[SecuritySafeCritical]
		public void Invoke()
		{
			InvokeNativeInternal();
		}

		[SecurityCritical]
		private void InvokeNativeInternal()
		{
			unsafe
			{
				fixed (fxScriptContext* cxt = &m_extContext)
				{
					Helpers.InvokeNative(new IntPtr(cxt));
				}
			}
		}

		public unsafe byte[] GetBytes()
		{
			fixed (fxScriptContext* context = &m_extContext)
			{
				byte[] arr = new byte[8 * 32];
				Marshal.Copy((IntPtr)context->functionData, arr, 0, 8 * 32);

				return arr;
			}
		}

		public unsafe IntPtr GetContextUnderlyingAddress()
		{
			fixed (fxScriptContext* context = &m_extContext)
			{
				return (IntPtr)context;
			}
		}

		[SecuritySafeCritical]
		public void Push(object arg)
		{
			PushInternal(arg);
		}

        [SecuritySafeCritical]
		public unsafe void SetResult(object arg, fxScriptContext* cxt)
		{
            SetResultInternal(cxt, arg);
		}

        [SecurityCritical]
		private unsafe void PushInternal(object arg)
		{
			fixed (fxScriptContext* context = &m_extContext)
			{
				Push(context, arg);
			}
		}

		[SecurityCritical]
		public unsafe void SetIdentifier(ulong arg)
		{
			fixed (fxScriptContext* context = &m_extContext)
			{
				context->nativeIdentifier = arg;
			}
		}

		public unsafe void CheckErrors()
		{
			fixed (fxScriptContext* context = &m_extContext)
			{
				if (Convert.ToBoolean(context->hasError))
				{
					string error = GetResult<string>();
					Reset();
					throw new NativeException(error);
				}
			}

		}

		[SecurityCritical]
		internal unsafe void Push(fxScriptContext* context, object arg)
		{
			if (arg == null)
			{
				arg = 0;
			}

			if (arg.GetType().IsEnum)
			{
				arg = Convert.ChangeType(arg, arg.GetType().GetEnumUnderlyingType());
			}

			if (arg is string)
			{
				var str = (string)Convert.ChangeType(arg, typeof(string));
				PushString(context, str);

				return;
			}
			else if (arg is InputArgument ia)
			{
				Push(context, ia.Value);

				return;
			}

			if (Marshal.SizeOf(arg.GetType()) <= 8)
			{
				PushUnsafe(context, arg);
			}

			context->numArguments++;
		}

        [SecurityCritical]
        internal unsafe void SetResultInternal(fxScriptContext* context, object arg)
        {
            if (arg == null)
            {
                arg = 0;
            }

            if (arg.GetType().IsEnum)
            {
                arg = Convert.ChangeType(arg, arg.GetType().GetEnumUnderlyingType());
            }

            if (arg is string)
            {
                var str = (string)Convert.ChangeType(arg, typeof(string));
                SetResultString(context, str);

                return;
            }
            else if (arg is InputArgument ia)
            {
                SetResultInternal(context, ia.Value);

                return;
            }

            if (Marshal.SizeOf(arg.GetType()) <= 8)
            {
                SetResultUnsafe(context, arg);
            }
        }

		[SecurityCritical]
		internal unsafe void PushUnsafe(fxScriptContext* cxt, object arg)
		{
			*(long*)(&cxt->functionData[8 * cxt->numArguments]) = 0;
			Marshal.StructureToPtr(arg, new IntPtr(cxt->functionData + (8 * cxt->numArguments)), true);
		}

        [SecurityCritical]
        internal unsafe void SetResultUnsafe(fxScriptContext* cxt, object arg)
        {
            *(long*)(&cxt->result[0]) = 0;
            Marshal.StructureToPtr(arg, new IntPtr(cxt->result), true);
        }

		[SecuritySafeCritical]
		internal unsafe void PushFast<T>(fxScriptContext* cxt, T arg)
			where T : struct
		{
			var size = FastStructure<T>.Size;

			var numArgs = (size / 8);

			if ((size % 8) != 0)
			{
				*(long*)(&cxt->functionData[8 * cxt->numArguments]) = 0;
				numArgs++;
			}

			FastStructure<T>.StructureToPtr(ref arg, new IntPtr(&cxt->functionData[8 * cxt->numArguments]));

			cxt->numArguments += numArgs;
		}

		[SecurityCritical]
		internal unsafe T GetResultFast<T>(fxScriptContext* cxt)
			where T : struct
		{
			return FastStructure<T>.PtrToStructure(new IntPtr(&cxt->functionData));
		}

		[SecurityCritical]
		internal unsafe void PushString(string str)
		{
			fixed (fxScriptContext* cxt = &m_extContext)
			{
				PushString(cxt, str);
			}
		}

		[SecurityCritical]
		internal unsafe void PushString(fxScriptContext* cxt, string str)
		{
			var ptr = IntPtr.Zero;

			if (str != null)
			{
				var b = Encoding.UTF8.GetBytes(str);

				ptr = Marshal.AllocHGlobal(b.Length + 1);

				Marshal.Copy(b, 0, ptr, b.Length);
				Marshal.WriteByte(ptr, b.Length, 0);

				ms_finalizers.Enqueue(() => Free(ptr));
			}

			unsafe
			{
				*(IntPtr*)(&cxt->functionData[8 * cxt->numArguments]) = ptr;
			}

			cxt->numArguments++;
		}

        [SecurityCritical]
        internal unsafe void SetResultString(fxScriptContext* cxt, string str)
        {
            var ptr = IntPtr.Zero;

            if (str != null)
            {
                var b = Encoding.UTF8.GetBytes(str);

                ptr = Marshal.AllocHGlobal(b.Length + 1);

                Marshal.Copy(b, 0, ptr, b.Length);
                Marshal.WriteByte(ptr, b.Length, 0);

                ms_finalizers.Enqueue(() => Free(ptr));
            }

            unsafe
            {
                *(IntPtr*)(&cxt->result[8]) = ptr;
            }
        }

		[SecuritySafeCritical]
		private void Free(IntPtr ptr)
		{
			Marshal.FreeHGlobal(ptr);
		}

		[SecuritySafeCritical]
		public T GetArgument<T>(int index)
		{
			return (T)GetArgument(typeof(T), index);
		}

		[SecuritySafeCritical]
		public object GetArgument(Type type, int index)
		{
			return GetArgumentHelper(type, index);
		}

		[SecurityCritical]
		internal unsafe object GetArgument(fxScriptContext* cxt, Type type, int index)
		{
			return GetArgumentHelper(cxt, type, index);
		}

		[SecurityCritical]
		private unsafe object GetArgumentHelper(Type type, int index)
		{
			fixed (fxScriptContext* cxt = &m_extContext)
			{
				return GetArgumentHelper(cxt, type, index);
			}
		}

		[SecurityCritical]
		private unsafe object GetArgumentHelper(fxScriptContext* context, Type type, int index)
		{
			return GetResult(type, &context->functionData[index * 8]);
		}

		[SecuritySafeCritical]
		public T GetResult<T>()
		{
			return (T)GetResult(typeof(T));
		}

		[SecuritySafeCritical]
		public object GetResult(Type type)
		{
			return GetResultHelper(type);
		}

		[SecurityCritical]
		internal unsafe object GetResult(fxScriptContext* cxt, Type type)
		{
			return GetResultHelper(cxt, type);
		}

		[SecurityCritical]
		private unsafe object GetResultHelper(Type type)
		{
			fixed (fxScriptContext* cxt = &m_extContext)
			{
				return GetResultHelper(cxt, type);
			}
		}

		[SecurityCritical]
		private unsafe object GetResultHelper(fxScriptContext* context, Type type)
		{
			return GetResult(type, &context->result[0]);
		}

		[SecurityCritical]
		internal unsafe object GetResult(Type type, byte* ptr)
		{
			if (type == typeof(string))
			{
				var nativeUtf8 = *(IntPtr*)&ptr[0];

				if (nativeUtf8 == IntPtr.Zero)
				{
					return null;
				}

				var len = 0;
				while (Marshal.ReadByte(nativeUtf8, len) != 0)
				{
					++len;
				}

				var buffer = new byte[len];
				Marshal.Copy(nativeUtf8, buffer, 0, buffer.Length);
				return Encoding.UTF8.GetString(buffer);
			}

			if (typeof(NativeObject).IsAssignableFrom(type))
			{
				var pointer = (IntPtr)GetResult(typeof(IntPtr), ptr);
				return Activator.CreateInstance(type, pointer);
			}

			if (type == typeof(object))
			{
				var dataPtr = *(IntPtr*)&ptr[0];
				var dataLength = *(long*)&ptr[8];

				byte[] data = new byte[dataLength];
				Marshal.Copy(dataPtr, data, 0, (int)dataLength);

				return null;
				//return MsgPackDeserializer.Deserialize(data);
			}

			if (type.IsEnum)
			{
				return Enum.ToObject(type, (int)GetResult(typeof(int), ptr));
			}

			/*if (type.IsAssignableFrom(typeof(INativeValue)))
			{
				var a = (int)GetResultInternal(typeof(int), ptr);

				return Activator.CreateInstance(type, a);
			}

			if (type == typeof(Vector3))
			{
				var x = *(float*)(&ptr[0]);
				var y = *(float*)(&ptr[8]);
				var z = *(float*)(&ptr[16]);

				return new Vector3(x, y, z);
			}*/

			if (Marshal.SizeOf(type) <= 8)
			{
				return GetResultInternal(type, ptr);
			}

			return null;
		}

		[SecurityCritical]
		private unsafe object GetResultInternal(Type type, byte* ptr)
		{
			var obj = Marshal.PtrToStructure(new IntPtr(ptr), type);
			return obj;
		}


		[SecurityCritical]
		internal unsafe string ErrorHandler(byte* error)
		{
			if (error != null)
			{
				var errorStart = error;
				int length = 0;

				for (var p = errorStart; *p != 0; p++)
				{
					length++;
				}

				return Encoding.UTF8.GetString(errorStart, length);
			}

			return "Native invocation failed.";
		}

		internal void GlobalCleanUp()
		{
			lock (ms_lock)
			{
				while (ms_finalizers.TryDequeue(out var cb))
				{
					cb();
				}
			}
		}

		public override string ToString()
		{
			return $"ScriptContext{{numArgs={m_extContext.numArguments}}}";
		}
	}
}