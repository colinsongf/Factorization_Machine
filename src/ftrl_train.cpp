// Copyright (c) 2014-2015 The AsyncFTRL Project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <getopt.h>
#include <unistd.h>
#include <iostream>
#include <locale>
#include "src/fast_ftrl_solver.h"
#include "src/ftrl_train.h"
#include "src/util.h"

using namespace std;

void print_usage() {
	printf("Usage: ./ftrl_train -f input_file -m model_file [options]\n"
		"options:\n"
		"--epoch iteration : set number of iteration, default 1\n"
		"--alpha alpha : set alpha param, default 0.15\n"
		"--beta beta : set beta param, default 1\n"
		"--l1 l1 : set l1 param, default 1\n"
		"--l2 l2 : set l2 param, default 1\n"
		"--dropout dropout : set dropout rate, default 0\n"
		"--sync-step step : set push/fetch step of async ftrl, default 3\n"
		" thread on async model, default 0\n"
		"--start-from model_file : set to continue training from model_file\n"
		"--thread num : set thread num, default is single thread. 0 will use hardware concurrency\n"
		"--double-precision : set to use double precision, default false\n"
		"--batch_size : set num of samples load in batch\n"
		"--is_rank: choose rank loss by set is_rank as 1 or logistic loss by set is_rank as 0\n"
		"--help : print this help\n"

	);
}


template<typename T>
bool train(const char* input_file,  const char* model_file,
		const char* start_from_model, T alpha, T beta, T l1, T l2, T dropout,
		size_t epoch, size_t push_step, size_t fetch_step, size_t num_threads, 
		int batch_size,int is_rank) {

		FastFtrlTrainer<T> trainer;
		trainer.Initialize(epoch, num_threads,  push_step, fetch_step,is_rank);

		if (start_from_model) {
			trainer.Train(start_from_model,
				model_file, input_file);
		} else {
			trainer.Train(alpha, beta, l1, l2, dropout,
				model_file, input_file);
		}
	return true;
}

int main(int argc, char* argv[]) {
	int opt;
	int opt_idx = 0;

	static struct option long_options[] = {
		{"epoch", required_argument, NULL, 'i'},
		{"alpha", required_argument, NULL, 'a'},
		{"beta", required_argument, NULL, 'b'},
		{"dropout", required_argument, NULL, 'd'},
		{"l1", required_argument, NULL, 'l'},
		{"l2", required_argument, NULL, 'e'},
		{"sync-step", required_argument, NULL, 's'},
		{"start-from", required_argument, NULL, 'r'},
		{"thread", required_argument, NULL, 'n'},
		{"double-precision", no_argument, NULL, 'x'},
		{"help", no_argument, NULL, 'h'},
		{"batch_size", required_argument, NULL, 'y'},
		{"is_rank", required_argument, NULL, 'p'},
		{0, 0, 0, 0}
	};

	std::string input_file;
	std::string test_file;
	std::string model_file;
	std::string start_from_model;

	double alpha = DEFAULT_ALPHA;
	double beta = DEFAULT_BETA;
	double l1 = DEFAULT_L1;
	double l2 = DEFAULT_L2;
	double dropout = 0;

	size_t epoch = 1;
	size_t push_step = kPushStep;
	size_t fetch_step = kFetchStep;
	size_t num_threads = 1;
    int batch_size  = 1000000;
    int is_rank = 0;

	bool double_precision = false;

	while ((opt = getopt_long(argc, argv, "f:m:ch", long_options, &opt_idx)) != -1) {
		switch (opt) {
		case 'f':
			input_file = optarg;
			break;
		case 't':
			test_file = optarg;
			break;
		case 'm':
			model_file = optarg;
			break;
		case 'i':
			epoch = (size_t)atoi(optarg);
			break;
		case 'a':
			alpha = atof(optarg);
			break;
		case 'b':
			beta = atof(optarg);
			break;
		case 'd':
			dropout = atof(optarg);
			break;
		case 'l':
			l1 = atof(optarg);
			break;
		case 'p':
			is_rank = atoi(optarg);
			break;
		case 'e':
			l2 = atof(optarg);
			break;
		case 's':
			push_step = (size_t)atoi(optarg);
			fetch_step = push_step;
			break;
		case 'n':
			num_threads = (size_t)atoi(optarg);
			break;
		case 'x':
			double_precision = true;
			break;
		case 'r':
			start_from_model = optarg;
			break;
		case 'y':
			batch_size = atoi(optarg);
			break;
		case 'h':
		default:
			print_usage();
			exit(0);
		}
	}

	if (input_file.size() == 0 || model_file.size() == 0) {
		print_usage();
		exit(1);
	}


	const char* pstart_from_model = NULL;
	if (start_from_model.size() > 0) pstart_from_model = start_from_model.c_str();

	if (double_precision) {
		train<double>(input_file.c_str(),  model_file.c_str(),
			pstart_from_model, alpha, beta, l1, l2, dropout,
			epoch, push_step, fetch_step, num_threads, batch_size,is_rank);
	} else {
		train<double>(input_file.c_str(),  model_file.c_str(),
			pstart_from_model, alpha, beta, l1, l2, dropout,
			epoch, push_step, fetch_step, num_threads, batch_size,is_rank);
	}

	return 0;
}
/* vim: set ts=4 sw=4 tw=0 noet :*/
