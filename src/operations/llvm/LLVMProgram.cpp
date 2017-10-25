#pragma once

#include <array>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>

//#define DEBUG
#ifdef DEBUG
#include <iostream>
#endif

template<typename DataType = float, size_t stack_size = 32 /  sizeof(DataType), size_t n_op_arg = (sizeof(DataType) / sizeof(uint8_t))>
class LLVMProgram {
public:
	enum OpCode {
		BRK,
		LDC,
		LDS,
		POP,
		ADD,
		SUB,
		MUL,
		DIV,
		SIN,
		COS,
		MIN,
		MAX,
		_NUM_OPS
	};


	private:
	
	std::vector<OpCode> code;

	using const_code_it = typename std::vector<OpCode>::const_iterator;
	using const_data_it = typename std::array<DataType, stack_size>::const_iterator;
	using data_it = typename std::array<DataType, stack_size>::iterator;
	using data_rit = typename std::array<DataType, stack_size>::reverse_iterator;

	template<typename S, typename I>
	inline void print_stack(S& stack, I& stack_it) const {
		#ifdef DEBUG
		for (auto it = stack.begin(); it != stack.end(); it++) {
			std::cerr << "\t" << (it == stack_it? "->" : "  ") << *it << std::endl;
		}
		#endif
	}

	
	inline DataType from_n_args(const_code_it& ops) const {
		union { uint8_t b[n_op_arg]; DataType d; } tmp;
		for (size_t i = 0; i < n_op_arg; i++)
			tmp.b[i] = *ops++;
		return tmp.d;
	}

	template<typename F>
	inline void do_binary(data_it& stack_it, F op) const {
		const auto a = *(stack_it--);
		const auto b = *(stack_it);
		const auto ret = op(a, b);
		*(stack_it) = ret;
	}

	template<typename F>
	inline void do_unary(data_it& stack_it, F op) const {
		const auto a = *(stack_it);
		*(stack_it) = op(a);
	}

	public:

	LLVMProgram(const std::vector<uint8_t>& ucode) {
		code.resize(ucode.size());
		std::transform(ucode.begin(), ucode.end(), code.begin(), [](uint8_t v){
			return OpCode(v);
		});
	}

	DataType run(std::vector<DataType> arguments) const {
		std::array<DataType, stack_size> stack;
		std::fill(stack.begin(), stack.end(), 0.0f);
		data_it stack_it = stack.begin();

		for (const auto& arg : arguments) {
			*(++stack_it) = arg;
		}
		//if (arguments.size() == 0) stack_it ++;
		
		for (const_code_it code_it = code.cbegin(); code_it != code.cend(); ++code_it) {
			const auto op = *code_it;
			#ifdef DEBUG
			std::cerr << "OP: " << op << std::endl;
			std::cerr << "stack: " << std::endl;
			#endif
			print_stack(stack, stack_it);
			switch (uint8_t(op)) {
				case OpCode::BRK:
					return *stack_it;
				case OpCode::LDC:
					*(stack_it++) = from_n_args(code_it); break;
				case OpCode::LDS: {
					const size_t offset = *(code_it++);
					DataType d = *(stack_it - offset);
					*(stack_it++) = d;
				}; break;
				case OpCode::ADD: do_binary(stack_it, [](DataType a, DataType b) { return a + b; }); break;
				case OpCode::MUL: do_binary(stack_it, [](DataType a, DataType b) { return a * b; }); break;
				case OpCode::SUB: do_binary(stack_it, [](DataType a, DataType b) { return a - b; }); break;
				case OpCode::DIV: do_binary(stack_it, [](DataType a, DataType b) { return a / b; }); break;
				case OpCode::SIN: do_unary(stack_it, [](DataType a) { return std::sin(a); }); break;
				case OpCode::COS: do_unary(stack_it, [](DataType a) { return std::cos(a); }); break;
				case OpCode::MIN: do_binary(stack_it, [](DataType a, DataType b) { return std::min(a, b); }); break;
				case OpCode::MAX: do_binary(stack_it, [](DataType a, DataType b) { return std::max(a, b); }); break;
			}
		}
		print_stack(stack, stack_it);
	
		return *stack_it;
	}
};

#if 0

int main(int argc, char** argv) {
	std::vector<uint8_t> code = {
		(uint8_t)LLVMProgram<>::OpCode::ADD,
		(uint8_t)LLVMProgram<>::OpCode::MUL,
		(uint8_t)LLVMProgram<>::OpCode::BRK,
		(uint8_t)LLVMProgram<>::OpCode::MUL,
		(uint8_t)LLVMProgram<>::OpCode::DIV,
		(uint8_t)LLVMProgram<>::OpCode::COS,
		(uint8_t)LLVMProgram<>::OpCode::SIN,
		(uint8_t)LLVMProgram<>::OpCode::MAX,
		(uint8_t)LLVMProgram<>::OpCode::MIN,
	};
	LLVMProgram<> p(code);
	assert(p.run({ 2, 2, 2 }) == 8);
}

#endif
