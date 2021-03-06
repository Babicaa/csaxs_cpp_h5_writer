#include <iostream>
#include <sstream>
#include <stdexcept>

#include "cpp_h5_writer/config.hpp"
#include "cpp_h5_writer/ProcessManager.hpp"
#include "cpp_h5_writer/WriterManager.hpp"
#include "cpp_h5_writer/ZmqReceiver.hpp"

#include "CsaxsFormat.cpp"

int main (int argc, char *argv[])
{
    if (argc != 6) {
        cout << endl;
        cout << "Usage: csaxs_cpp_h5_writer [connection_address] [output_file] [n_frames] [rest_port] [user_id]" << endl;
        cout << "\tconnection_address: Address to connect to the stream (PULL). Example: tcp://127.0.0.1:40000" << endl;
        cout << "\toutput_file: Name of the output file." << endl;
        cout << "\tn_frames: Number of images to acquire. 0 for infinity (untill /stop is called)." << endl;
        cout << "\trest_port: Port to start the REST Api on." << endl;
        cout << "\tuser_id: uid under which to run the writer. -1 to leave it as it is." << endl;
        cout << endl;

        exit(-1);
    }

    // This process can be set to run under a different user.
    auto user_id = atoi(argv[5]);
    if (user_id != -1) {

        #ifdef DEBUG_OUTPUT
            cout << "[csaxs_cpp_h5_writer::main] Setting process uid to " << user_id << endl;
        #endif

        if (setuid(user_id)) {
            stringstream error_message;
            error_message << "[csaxs_cpp_h5_writer::main] Cannot set user_id to " << user_id << endl;

            throw runtime_error(error_message.str());
        }
    }

    int n_frames =  atoi(argv[3]);
    string output_file = string(argv[2]);

    CsaxsFormat format;
    
    WriterManager manager(format.get_input_value_type(), output_file, n_frames);

    string connect_address = string(argv[1]);
    int n_io_threads = config::zmq_n_io_threads;
    int receive_timeout = config::zmq_receive_timeout;
    ZmqReceiver receiver(connect_address, n_io_threads, receive_timeout);

    int rest_port = atoi(argv[4]);

    ProcessManager::run_writer(manager, format, receiver, rest_port);

    return 0;
}
