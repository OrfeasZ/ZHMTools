#pragma once

class XTEA
{
public:
    // From RPKG https://github.com/glacier-modding/RPKG-Tool/blob/145d8d7d9711d57f1434489706c3d81b2feeed73/src/crypto.cpp#L3
    static constexpr uint32_t c_L10nKey[4] = { 0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268 };
    static constexpr uint32_t c_L10nDelta = 0x9E3779B9;
    static constexpr size_t c_L10nRounds = 32;

    static constexpr size_t c_BlockSize = sizeof(uint32_t) * 2;

	static void DecryptInPlace(void* p_Memory, size_t p_Bytes, size_t p_Rounds, const uint32_t p_Key[4], uint32_t p_Delta)
	{
        for (size_t s_Block = 0; s_Block < p_Bytes / c_BlockSize; ++s_Block)
        {
	        const auto s_V0Ptr = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(p_Memory) + (s_Block * c_BlockSize));
	        const auto s_V1Ptr = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(p_Memory) + (s_Block * c_BlockSize) + sizeof(uint32_t));

            auto s_V0 = *s_V0Ptr;
            auto s_V1 = *s_V1Ptr;
            
            uint32_t s_Sum = p_Delta * static_cast<uint32_t>(p_Rounds);

            for (size_t i = 0; i < p_Rounds; ++i) 
            {
                s_V1 -= (((s_V0 << 4) ^ (s_V0 >> 5)) + s_V0) ^ (s_Sum + p_Key[(s_Sum >> 11) & 3]);
                s_Sum -= p_Delta;
                s_V0 -= (((s_V1 << 4) ^ (s_V1 >> 5)) + s_V1) ^ (s_Sum + p_Key[s_Sum & 3]);
            }

            *s_V0Ptr = s_V0;
            *s_V1Ptr = s_V1;
        }
	}

	static std::string Encrypt(std::string_view p_Data, size_t p_Rounds, const uint32_t p_Key[4], uint32_t p_Delta)
	{
        const auto s_EncryptedSize = p_Data.size() + (c_BlockSize - (p_Data.size() % c_BlockSize));

        if (s_EncryptedSize == 0)
            return "";

        std::string s_EncryptedData { p_Data };
        s_EncryptedData.resize(s_EncryptedSize, '\0');

        for (size_t s_Block = 0; s_Block < s_EncryptedSize / c_BlockSize; ++s_Block)
        {
            const auto s_V0Ptr = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(s_EncryptedData.data()) + (s_Block * c_BlockSize));
            const auto s_V1Ptr = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(s_EncryptedData.data()) + (s_Block * c_BlockSize) + sizeof(uint32_t));

            auto s_V0 = *s_V0Ptr;
            auto s_V1 = *s_V1Ptr;
            
            uint32_t s_Sum = 0;

            for (size_t i = 0; i < p_Rounds; ++i) 
            {
                s_V0 += (((s_V1 << 4) ^ (s_V1 >> 5)) + s_V1) ^ (s_Sum + p_Key[s_Sum & 3]);
                s_Sum += p_Delta;
                s_V1 += (((s_V0 << 4) ^ (s_V0 >> 5)) + s_V0) ^ (s_Sum + p_Key[(s_Sum >> 11) & 3]);
            }

            *s_V0Ptr = s_V0;
            *s_V1Ptr = s_V1;
        }

        return s_EncryptedData;
	}
};