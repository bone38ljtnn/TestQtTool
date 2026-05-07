#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>

class MyClass {
public:
	int id;
	std::string name;
	std::vector<double> data;

	// 序列化方法
	template<class Archive>
	void serialize(Archive& archive) {
		archive(id, name, data);
	}

	// 保存到文件
	bool save(const std::string& filename) const {
		try {
			std::ofstream ofs(filename, std::ios::binary);
			cereal::BinaryOutputArchive archive(ofs);
			archive(*this);
			return true;
		}
		catch (...) {
			return false;
		}
	}

	// 从文件加载
	bool load(const std::string& filename) {
		try {
			std::ifstream ifs(filename, std::ios::binary);
			cereal::BinaryInputArchive archive(ifs);
			archive(*this);
			return true;
		}
		catch (...) {
			return false;
		}
	}
};

// 使用示例
void example() {
	MyClass obj1;
	obj1.id = 42;
	obj1.name = "Test";
	for (int i = 0; i < 1e5; ++i) {
		obj1.data.emplace_back(i * 1.2);
	}

	// 保存
	obj1.save("data.bin");

	// 读取
	MyClass obj2;
	obj2.load("data.bin");
	// obj2 完美复原 obj1 的内容
}

int main() {
	example();

	return 0;
}