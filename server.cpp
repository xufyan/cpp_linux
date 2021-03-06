#include <XSMBServiceHandler.h>
#include <FileShareServiceHandler.h>

#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/protocol/TMultiplexedProtocol.h"
#include "thrift/TProcessor.h"

#include "thrift/server/TNonblockingServer.h"
#include "thrift/concurrency/PlatformThreadFactory.h"

#include "thrift/transport/TServerSocket.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/processor/TMultiplexedProcessor.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <UnitTest.h>
#include <thread>

using namespace azure::storage::cpp_linux;

using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

int run_server(int port, unsigned int threadCount, bool logOn) {

	boost::shared_ptr<XSMBServiceHandler> xSMBServiceHandler(new XSMBServiceHandler(logOn));
	boost::shared_ptr<TProcessor> xSMBServiceProcessor(new XSMBServiceProcessor(xSMBServiceHandler));

	boost::shared_ptr<FileShareServiceHandler> fileShareServiceHandler(new FileShareServiceHandler(logOn));
	boost::shared_ptr<TProcessor> fileShareServiceProcessor(new FileShareServiceProcessor(fileShareServiceHandler));
	
	boost::shared_ptr<TMultiplexedProcessor> multiplexedProcessor(new TMultiplexedProcessor());
	multiplexedProcessor->registerProcessor("XSMBService", xSMBServiceProcessor);
	multiplexedProcessor->registerProcessor("FileShareService", fileShareServiceProcessor);

	boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	boost::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(threadCount);
	boost::shared_ptr<PlatformThreadFactory> threadFactory = boost::shared_ptr<PlatformThreadFactory>(new PlatformThreadFactory());
	threadManager->threadFactory(threadFactory);
	threadManager->start();
	TNonblockingServer server(multiplexedProcessor, protocolFactory, port, threadManager);
	server.serve();
	return 0;
}

int main(int argc, char* argv[]) {
	int port = 0;
	unsigned int threadCount = 0U;
	bool logOn = false;
	// Parse command line options
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-l") == 0) {
			logOn = true;
		}

		if (strcmp(argv[i], "-p") == 0)
		{
			if (++i >= argc)
			{
				std::cerr << "Value missing for argument: -p" << std::endl;
				return -1;
			}
			port = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-t") == 0)
		{
			if (++i >= argc)
			{
				std::cerr << "Value missing for argument: -t" << std::endl;
			}
			threadCount = atoi(argv[i]);
		}
	}

	if (threadCount == 0)
	{
		threadCount = std::thread::hardware_concurrency() * 2;
	}

	return run_server(port, threadCount, logOn);
}

