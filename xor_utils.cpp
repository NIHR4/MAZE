#include "xor_utils.h"
CipherPool::CipherPool(const std::string cipher) : m_cipher([cipher] {std::vector<unsigned char>tv(cipher.begin(), cipher.end()); return tv; }()) {}
CipherPool::CipherPool(std::vector<unsigned char>cipher) : m_cipher(cipher) {};
char CipherPool::get() {
	if (cipher_pos == m_cipher.size()) cipher_pos = 0;
	return m_cipher[cipher_pos++];
}

std::vector<unsigned char> perform_xor(std::string in, std::vector<unsigned char>& cipher_vec) {
	CipherPool pool(cipher_vec);
	std::vector<unsigned char>out;
	for (auto& c : in)  out.push_back(c ^ pool.get());
	return out;
}

std::vector<unsigned char> perform_xor(std::vector<unsigned char> in, std::vector<unsigned char> & cipher_vec) {
	CipherPool pool(cipher_vec);
	std::vector<unsigned char>out;
	for (auto& c : in)  out.push_back(c ^ pool.get());
	return out;
}

const std::string vec_to_str(std::vector<unsigned char>vec) {
	std::string str(vec.begin(), vec.end());
	return str;
}
const std::wstring vec_to_wstr(std::vector<unsigned char>vec) {
	std::wstring wstr(vec.begin(), vec.end());
	return wstr;
}