using System;
using System.IO;
using System.Reflection.Metadata.Ecma335;
using ZHM.Common.IO;
using ZHM.Serialization;

namespace ResourceParser
{
    class Program
    {
        static void Main(string[] args)
        {
            //var s_Path = "C:\\Research\\HITMAN3\\chunk3\\TEMP\\[00D17645CAA28C8A]mission_bulldog.brick";
            //var s_Path = "C:\\Research\\HITMAN3\\chunk3\\TEMP\\[0093F159C450E5EC]vr_overrides_ps4perf.brick";
            //var s_Path = "C:\\Research\\HITMAN3\\chunk3\\TEMP\\[004E93563D0C209F]scene_bulldog.entity";
            //var s_Path = "C:\\Research\\HITMAN3\\chunk0\\TEMP\\00CBDB260924C51A.TEMP";
            var s_Path = "C:\\Research\\HITMAN3\\chunk0\\TEMP\\003864F5A729149F.TEMP";

            using var s_File = File.Open(s_Path, FileMode.Open);

            Console.WriteLine($"Parsing file '{s_Path}'.");
            
            using var s_Deserialize = new BinaryDeserializer(s_File);

            var s_Thing = s_Deserialize.Deserialize<int>();
        }
    }
}
