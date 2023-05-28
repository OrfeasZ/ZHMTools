using System.Runtime.InteropServices;

/**
 * ResourceLib .NET bindings
 *
 * Example usage:
 *
 * if (ResourceLib.IsResourceTypeSupported("TEMP", ResourceLib.Game.Hitman3))
 * {
 *     // Create a new converter to convert game resources to json.
 *     var s_Converter = new ResourceLib.ResourceConverter("TEMP", ResourceLib.Game.Hitman3);
 *     var s_Json = s_Converter.FromResourceFileToJsonString("C:\\somewhere\\0000006196A6C097.TEMP");
 *     Console.WriteLine(s_Json);
 *     
 *     // Create a new generator to convert json to game resource data.
 *     var s_Generator = new ResourceLib.ResourceGenerator("TEMP", ResourceLib.Game.Hitman3);
 *     var s_ResourceMem = s_Generator.FromJsonStringToResourceMem(s_Json);
 * }
 * 
 * This file (ResourceLib.cs) is licensed under the MIT license.
 *
 * Copyright (c) 2023 Orfeas Zafeiris
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

public static class ResourceLib
{
    private static class Native
    {
        // ResourceLib exports.
        // HM3
        [DllImport("ResourceLib_HM3.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr HM3_GetConverterForResource(string p_ResourceType);

        [DllImport("ResourceLib_HM3.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr HM3_GetGeneratorForResource(string p_ResourceType);

        [DllImport("ResourceLib_HM3.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr HM3_GetSupportedResourceTypes();

        [DllImport("ResourceLib_HM3.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void HM3_FreeSupportedResourceTypes(IntPtr p_Array);

        [DllImport("ResourceLib_HM3.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern bool HM3_IsResourceTypeSupported(string p_ResourceType);

        // HM2
        [DllImport("ResourceLib_HM2.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr HM2_GetConverterForResource(string p_ResourceType);

        [DllImport("ResourceLib_HM2.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr HM2_GetGeneratorForResource(string p_ResourceType);

        [DllImport("ResourceLib_HM2.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr HM2_GetSupportedResourceTypes();

        [DllImport("ResourceLib_HM2.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void HM2_FreeSupportedResourceTypes(IntPtr p_Array);

        [DllImport("ResourceLib_HM2.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern bool HM2_IsResourceTypeSupported(string p_ResourceType);

        // HM2016
        [DllImport("ResourceLib_HM2016.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr HM2016_GetConverterForResource(string p_ResourceType);

        [DllImport("ResourceLib_HM2016.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr HM2016_GetGeneratorForResource(string p_ResourceType);

        [DllImport("ResourceLib_HM2016.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr HM2016_GetSupportedResourceTypes();

        [DllImport("ResourceLib_HM2016.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void HM2016_FreeSupportedResourceTypes(IntPtr p_Array);

        [DllImport("ResourceLib_HM2016.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern bool HM2016_IsResourceTypeSupported(string p_ResourceType);

        // ResourceConverter method declarations.
        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public delegate bool FromResourceFileToJsonFileDelegate(string p_ResourceFilePath, string p_OutputFilePath);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate bool FromMemoryToJsonFileDelegate(IntPtr p_ResourceData, UIntPtr p_Size, string p_OutputFilePath);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public delegate IntPtr FromResourceFileToJsonStringDelegate(string p_ResourceFilePath);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate IntPtr FromMemoryToJsonStringDelegate(IntPtr p_ResourceData, UIntPtr p_Size);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void FreeJsonStringDelegate(IntPtr p_JsonString);

        // ResourceGenerator method declarations.
        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public delegate bool FromJsonFileToResourceFileDelegate(string p_JsonFilePath, string p_ResourceFilePath, bool p_GenerateCompatible);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public delegate bool FromJsonStringToResourceFileDelegate(string p_JsonStr, UIntPtr p_JsonStrLength, string p_ResourceFilePath, bool p_GenerateCompatible);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public delegate IntPtr FromJsonFileToResourceMemDelegate(string p_JsonFilePath, bool p_GenerateCompatible);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public delegate IntPtr FromJsonStringToResourceMemDelegate(string p_JsonStr, UIntPtr p_JsonStrLength, bool p_GenerateCompatible);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void FreeResourceMemDelegate(IntPtr p_ResourceMem);

        // Structs.
        [StructLayout(LayoutKind.Sequential)]
        public struct JsonString
        {
            public IntPtr JsonData;
            public UIntPtr StrSize;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ResourceMem
        {
            public IntPtr ResourceData;
            public UIntPtr DataSize;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ResourceTypesArray
        {
            public IntPtr Types;
            public UIntPtr TypeCount;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ResourceConverter
        {
            public FromResourceFileToJsonFileDelegate FromResourceFileToJsonFile;
            public FromMemoryToJsonFileDelegate FromMemoryToJsonFile;
            public FromResourceFileToJsonStringDelegate FromResourceFileToJsonString;
            public FromMemoryToJsonStringDelegate FromMemoryToJsonString;
            public FreeJsonStringDelegate FreeJsonString;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ResourceGenerator
        {
            public FromJsonFileToResourceFileDelegate FromJsonFileToResourceFile;
            public FromJsonStringToResourceFileDelegate FromJsonStringToResourceFile;
            public FromJsonFileToResourceMemDelegate FromJsonFileToResourceMem;
            public FromJsonStringToResourceMemDelegate FromJsonStringToResourceMem;
            public FreeResourceMemDelegate FreeResourceMem;
        }
    }

    public enum Game
    {
        Hitman2016,
        Hitman2,
        Hitman3,
    }

    public class ResourceConverter
    {
        private readonly Native.ResourceConverter m_NativeConverter;
        
        public ResourceConverter(string p_ResourceType, Game p_Game)
        {
            var s_ConverterPtr = p_Game switch
            {
                Game.Hitman2016 => Native.HM2016_GetConverterForResource(p_ResourceType),
                Game.Hitman2 => Native.HM2_GetConverterForResource(p_ResourceType),
                Game.Hitman3 => Native.HM3_GetConverterForResource(p_ResourceType),
                _ => throw new ArgumentOutOfRangeException(nameof(p_Game), p_Game, null)
            };
            
            if (s_ConverterPtr == IntPtr.Zero)
                throw new Exception($"ResourceLib does not support converting '{p_ResourceType}' resources for {p_Game}.");
            
            m_NativeConverter = Marshal.PtrToStructure<Native.ResourceConverter>(s_ConverterPtr);
        }
        
        public bool FromResourceFileToJsonFile(string p_ResourceFilePath, string p_OutputFilePath)
        {
            return m_NativeConverter.FromResourceFileToJsonFile(p_ResourceFilePath, p_OutputFilePath);
        }
        
        public bool FromMemoryToJsonFile(byte[] p_ResourceData, string p_OutputFilePath)
        {
            var s_ResourceDataPtr = Marshal.AllocHGlobal(p_ResourceData.Length);
            Marshal.Copy(p_ResourceData, 0, s_ResourceDataPtr, p_ResourceData.Length);
            
            var s_Result = m_NativeConverter.FromMemoryToJsonFile(s_ResourceDataPtr, (UIntPtr)p_ResourceData.Length, p_OutputFilePath);
            
            Marshal.FreeHGlobal(s_ResourceDataPtr);
            return s_Result;
        }
        
        public string FromResourceFileToJsonString(string p_ResourceFilePath)
        {
            var s_NativeJsonStringPtr = m_NativeConverter.FromResourceFileToJsonString(p_ResourceFilePath);
           
            if (s_NativeJsonStringPtr == IntPtr.Zero)
                throw new Exception($"Failed to convert '{p_ResourceFilePath}' to JSON.");
            
            var s_NativeJsonString = Marshal.PtrToStructure<Native.JsonString>(s_NativeJsonStringPtr);
            var s_JsonString = Marshal.PtrToStringUTF8(s_NativeJsonString.JsonData, (int)s_NativeJsonString.StrSize);
            m_NativeConverter.FreeJsonString(s_NativeJsonStringPtr);
            
            return s_JsonString;
        }
        
        public string FromMemoryToJsonString(byte[] p_ResourceData)
        {
            var s_ResourceDataPtr = Marshal.AllocHGlobal(p_ResourceData.Length);
            Marshal.Copy(p_ResourceData, 0, s_ResourceDataPtr, p_ResourceData.Length);
            
            var s_NativeJsonStringPtr = m_NativeConverter.FromMemoryToJsonString(s_ResourceDataPtr, (UIntPtr)p_ResourceData.Length);
            
            Marshal.FreeHGlobal(s_ResourceDataPtr);
            
            if (s_NativeJsonStringPtr == IntPtr.Zero)
                throw new Exception($"Failed to convert resource data to JSON.");
            
            var s_NativeJsonString = Marshal.PtrToStructure<Native.JsonString>(s_NativeJsonStringPtr);
            var s_JsonString = Marshal.PtrToStringUTF8(s_NativeJsonString.JsonData, (int)s_NativeJsonString.StrSize);
            m_NativeConverter.FreeJsonString(s_NativeJsonStringPtr);
            
            return s_JsonString;
        }
    }

    public class ResourceGenerator
    {
        private readonly Native.ResourceGenerator m_NativeGenerator;
        
        public ResourceGenerator(string p_ResourceType, Game p_Game)
        {
            var s_GeneratorPtr = p_Game switch
            {
                Game.Hitman2016 => Native.HM2016_GetGeneratorForResource(p_ResourceType),
                Game.Hitman2 => Native.HM2_GetGeneratorForResource(p_ResourceType),
                Game.Hitman3 => Native.HM3_GetGeneratorForResource(p_ResourceType),
                _ => throw new ArgumentOutOfRangeException(nameof(p_Game), p_Game, null)
            };
            
            if (s_GeneratorPtr == IntPtr.Zero)
                throw new Exception($"ResourceLib does not support generating '{p_ResourceType}' resources for {p_Game}.");
            
            m_NativeGenerator = Marshal.PtrToStructure<Native.ResourceGenerator>(s_GeneratorPtr);
        }
        
        public bool FromJsonFileToResourceFile(string p_JsonFilePath, string p_OutputFilePath, bool p_GenerateCompatible = false)
        {
            return m_NativeGenerator.FromJsonFileToResourceFile(p_JsonFilePath, p_OutputFilePath, p_GenerateCompatible);
        }
        
        public bool FromJsonStringToResourceFile(string p_JsonString, string p_OutputFilePath, bool p_GenerateCompatible = false)
        {
            return m_NativeGenerator.FromJsonStringToResourceFile(p_JsonString, (UIntPtr)p_JsonString.Length, p_OutputFilePath, p_GenerateCompatible);
        }
        
        public byte[] FromJsonFileToResourceMem(string p_JsonFilePath, bool p_GenerateCompatible = false)
        {
            var s_NativeResourceMemPtr = m_NativeGenerator.FromJsonFileToResourceMem(p_JsonFilePath, p_GenerateCompatible);
            
            if (s_NativeResourceMemPtr == IntPtr.Zero)
                throw new Exception($"Failed to convert '{p_JsonFilePath}' to resource data.");
            
            var s_NativeResourceMem = Marshal.PtrToStructure<Native.ResourceMem>(s_NativeResourceMemPtr);
            var s_ResourceMem = new byte[(int)s_NativeResourceMem.DataSize];
            Marshal.Copy(s_NativeResourceMem.ResourceData, s_ResourceMem, 0, (int)s_NativeResourceMem.DataSize);
            m_NativeGenerator.FreeResourceMem(s_NativeResourceMemPtr);
            
            return s_ResourceMem;
        }
        
        public byte[] FromJsonStringToResourceMem(string p_JsonString, bool p_GenerateCompatible = false)
        {
            var s_NativeResourceMemPtr = m_NativeGenerator.FromJsonStringToResourceMem(p_JsonString, (UIntPtr)p_JsonString.Length, p_GenerateCompatible);
            
            if (s_NativeResourceMemPtr == IntPtr.Zero)
                throw new Exception($"Failed to convert JSON to resource data.");
            
            var s_NativeResourceMem = Marshal.PtrToStructure<Native.ResourceMem>(s_NativeResourceMemPtr);
            var s_ResourceMem = new byte[(int)s_NativeResourceMem.DataSize];
            Marshal.Copy(s_NativeResourceMem.ResourceData, s_ResourceMem, 0, (int)s_NativeResourceMem.DataSize);
            m_NativeGenerator.FreeResourceMem(s_NativeResourceMemPtr);
            
            return s_ResourceMem;
        }
    }

    public static int[] GetSupportedResourceTypes(Game p_Game)
    {
        var s_SupportedResourcesPtr = p_Game switch
        {
            Game.Hitman2016 => Native.HM2016_GetSupportedResourceTypes(),
            Game.Hitman2 => Native.HM2_GetSupportedResourceTypes(),
            Game.Hitman3 => Native.HM3_GetSupportedResourceTypes(),
            _ => throw new ArgumentOutOfRangeException(nameof(p_Game), p_Game, null)
        };
        
        if (s_SupportedResourcesPtr == IntPtr.Zero)
            throw new Exception($"Could not get supported resource types for {p_Game}.");
        
        var s_SupportedResourcesArray = Marshal.PtrToStructure<Native.ResourceTypesArray>(s_SupportedResourcesPtr);
        
        var s_SupportedResources = new int[(int)s_SupportedResourcesArray.TypeCount];
        Marshal.Copy(s_SupportedResourcesArray.Types, s_SupportedResources, 0, (int)s_SupportedResourcesArray.TypeCount);

        switch (p_Game)
        {
            case Game.Hitman2016:
                Native.HM2016_FreeSupportedResourceTypes(s_SupportedResourcesPtr);
                break;
            
            case Game.Hitman2:
                Native.HM2_FreeSupportedResourceTypes(s_SupportedResourcesPtr);
                break;
            
            case Game.Hitman3:
                Native.HM3_FreeSupportedResourceTypes(s_SupportedResourcesPtr);
                break;
        }
        
        return s_SupportedResources;
    }
    
    public static bool IsResourceTypeSupported(string p_ResourceType, Game p_Game)
    {
        return p_Game switch
        {
            Game.Hitman2016 => Native.HM2016_IsResourceTypeSupported(p_ResourceType),
            Game.Hitman2 => Native.HM2_IsResourceTypeSupported(p_ResourceType),
            Game.Hitman3 => Native.HM3_IsResourceTypeSupported(p_ResourceType),
            _ => throw new ArgumentOutOfRangeException(nameof(p_Game), p_Game, null)
        };
    }
}