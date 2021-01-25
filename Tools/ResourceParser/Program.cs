using System;
using System.IO;
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
            var s_FilePath = "C:\\Research\\HITMAN3\\chunk0\\TEMP\\003864F5A729149F.TEMP";

            using var s_File = File.Open(s_FilePath, FileMode.Open);

            if (!BinaryDeserializer.IsBinaryResource(s_File))
            {
                Console.WriteLine($"Skipping file '{s_FilePath}'. It is not a binary resource.");
                return;
            }

            Console.WriteLine($"Parsing file '{s_FilePath}'.");

            using var s_Deserialize = new BinaryDeserializer(s_File);

            var s_Thing = s_Deserialize.Deserialize<int>();
        }

        static void ParseAllInDirectory(string p_Directory)
        {
            foreach (var s_FilePath in Directory.GetFiles(p_Directory, "*.TEMP"))
            {
                using var s_File = File.Open(s_FilePath, FileMode.Open);

                if (!BinaryDeserializer.IsBinaryResource(s_File))
                {
                    Console.WriteLine($"Skipping file '{s_FilePath}'. It is not a binary resource.");
                    continue;
                }

                Console.WriteLine($"Parsing file '{s_FilePath}'.");

                using var s_Deserialize = new BinaryDeserializer(s_File);

                var s_Thing = s_Deserialize.Deserialize<int>();
            }
        }
    }
}
