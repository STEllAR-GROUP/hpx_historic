void work_queue::server::init(XPI_Buffer_impl buf)
{
    receptor=buf;
}

void work_queue::server::wait(hpx::naming::id_type thread_gid)
{
    // put thread to sleep
    // Add thread to queue
    // pop item off queue, wake up threads
    //queue.schedule_thread()
}
