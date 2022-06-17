/**
 * This file contains implementations for the methods defined in the Simulation
 * class.
 *
 * You'll probably spend a lot of your time here.
 */

#include "simulation/simulation.h"
#include <stdexcept>

Simulation::Simulation(FlagOptions &flags)
{
    this->flags = flags;
    this->frames.reserve(this->NUM_FRAMES);
}

void Simulation::run()
{
    // TODO: implement me
    for (int i = 0; i < 512; i++)
    {
        free_frames.push_back(i);
    }
    int returnCode;
    for (VirtualAddress virtA : virtual_addresses)
    {
        returnCode = perform_memory_access(virtA);
        if (returnCode != 0)
        {
            break;
        }
    }
    if (returnCode == 0)
    {
        print_summary();
    }
    for (auto i = processes.begin(); i != processes.end(); i++)
    {
        for (int j = 0; j < i->second->pages.size(); j++)
        {
            delete i->second->pages[j];
        }
        delete i->second;
    }
}

char Simulation::perform_memory_access(const VirtualAddress &virtual_address)
{
    this->time++;
    if (this->flags.verbose)
    {
        std::cout << virtual_address << std::endl;
    }
    if (processes[virtual_address.process_id]->is_valid_page(virtual_address.page))
    {
        processes[virtual_address.process_id]->memory_accesses++;
        processes[virtual_address.process_id]->page_table.rows[virtual_address.page].last_accessed_at = time;
        //is page present? (no = handle_page_fault)
        if (!processes[virtual_address.process_id]->page_table.rows[virtual_address.page].present)
        {
            if (this->flags.verbose)
            {
                std::cout << "    -> PAGE FAULT" << std::endl;
            }
            Simulation::handle_page_fault(processes[virtual_address.process_id], virtual_address.page);
        }
        else
        {
            if (this->flags.verbose)
            {
                std::cout << "  -> IN MEMORY" << std::endl;
            }
        }
        PhysicalAddress physA(processes[virtual_address.process_id]->page_table.rows[virtual_address.page].frame, virtual_address.offset);
        if (this->flags.verbose)
        {
            std::cout << "  -> physical address " << physA << std::endl;
        }

        //processes->virtAddr.procId->PageTable->page.rows[virtAddr.page]

        //is offset valid (no = seg fault again)
        if (processes[virtual_address.process_id]->pages[virtual_address.page]->is_valid_offset(virtual_address.offset))
        {
            //get byte at offset
            //return processes[virtual_address.process_id]->pages[virtual_address.page]->get_byte_at_offset(virtual_address.offset);
        }
        else
        {
            std::cout << "SEGFAULT - INVALID OFFSET";
            return -1;
        }
        if (this->flags.verbose)
        {
            std::cout << "    -> RSS: " << processes[virtual_address.process_id]->get_rss() << std::endl;
        }
    }
    else
    {
        std::cout << "SEGFAULT - INVALID PAGE";
        return -1;
    }
    return 0;
}
//if(this->flags.verbose){std::cout<<""<<endl;}

void Simulation::handle_page_fault(Process *process, size_t page)
{
    //keep track of num page fault in simulation && per process
    this->page_faults++;
    process->page_faults++;
    process->page_table.rows[page].loaded_at = time;

    //if not full just add it
    if (flags.max_frames > process->numFrames)
    {
        //if
        //std::cout << "max: " << flags.max_frames << std::endl;

        process->numFrames++;
        //std::cout << "1: " << process->numFrames << std::endl;
        Frame newFrame;
        newFrame.set_page(process, page);
        frames.push_back(newFrame);
        process->page_table.rows[page].frame = this->free_frames.front();
        this->free_frames.pop_front();

        process->page_table.rows[page].present = true;
        //std::cout << "2: " << process->numFrames << std::endl;
    }
    else
    {
        size_t oldIndex;
        if (flags.strategy == ReplacementStrategy::FIFO)
        {
            //run get_oldest
            oldIndex = process->page_table.get_oldest_page();
            //process->page_table.rows[oldIndex].loaded_at = -1;
        }
        else if (flags.strategy == ReplacementStrategy::LRU)
        {
            oldIndex = process->page_table.get_least_recently_used_page();
            //run  get_lru
        }
        //std::cout << "3: " << process->numFrames << std::endl;

        process->page_table.rows[page].present = true;
        process->page_table.rows[oldIndex].present = false;

        process->page_table.rows[page].frame = process->page_table.rows[oldIndex].frame;

        this->frames[process->page_table.rows[oldIndex].frame].set_page(process, page);

        //std::cout << "4: " << process->numFrames << std::endl;
        //set present to false in old page
        //set present to true in new page
        //set new page on the frame the old page was in
        //frames.back().set_page(process, page);
    }
}

void Simulation::print_summary()
{
    if (!this->flags.csv)
    {
        boost::format process_fmt(
            "Process %3d:  "
            "ACCESSES: %-6lu "
            "FAULTS: %-6lu "
            "FAULT RATE: %-8.2f "
            "RSS: %-6lu\n");

        for (auto entry : this->processes)
        {
            std::cout << process_fmt % entry.first % entry.second->memory_accesses % entry.second->page_faults % entry.second->get_fault_percent() % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "\n%-25s %12lu\n"
            "%-25s %12lu\n"
            "%-25s %12lu\n");

        std::cout << summary_fmt % "Total memory accesses:" % this->virtual_addresses.size() % "Total page faults:" % this->page_faults % "Free frames remaining:" % this->free_frames.size();
    }

    if (this->flags.csv)
    {
        boost::format process_fmt(
            "%d,"
            "%lu,"
            "%lu,"
            "%.2f,"
            "%lu\n");

        for (auto entry : processes)
        {
            std::cout << process_fmt % entry.first % entry.second->memory_accesses % entry.second->page_faults % entry.second->get_fault_percent() % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "%lu,,,,\n"
            "%lu,,,,\n"
            "%lu,,,,\n");

        std::cout << summary_fmt % this->virtual_addresses.size() % this->page_faults % this->free_frames.size();
    }
}

int Simulation::read_processes(std::istream &simulation_file)
{
    int num_processes;
    simulation_file >> num_processes;

    for (int i = 0; i < num_processes; ++i)
    {
        int pid;
        std::string process_image_path;

        simulation_file >> pid >> process_image_path;

        std::ifstream proc_img_file(process_image_path);

        if (!proc_img_file)
        {
            std::cerr << "Unable to read file for PID " << pid << ": " << process_image_path << std::endl;
            return 1;
        }
        this->processes[pid] = Process::read_from_input(proc_img_file);
    }
    return 0;
}

int Simulation::read_addresses(std::istream &simulation_file)
{
    int pid;
    std::string virtual_address;

    try
    {
        while (simulation_file >> pid >> virtual_address)
        {
            this->virtual_addresses.push_back(VirtualAddress::from_string(pid, virtual_address));
        }
    }
    catch (const std::exception &except)
    {
        std::cerr << "Error reading virtual addresses." << std::endl;
        std::cerr << except.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Error reading virtual addresses." << std::endl;
        return 1;
    }
    return 0;
}

int Simulation::read_simulation_file()
{
    std::ifstream simulation_file(this->flags.filename);
    // this->simulation_file.open(this->flags.filename);

    if (!simulation_file)
    {
        std::cerr << "Unable to open file: " << this->flags.filename << std::endl;
        return -1;
    }
    int error = 0;
    error = this->read_processes(simulation_file);

    if (error)
    {
        std::cerr << "Error reading processes. Exit: " << error << std::endl;
        return error;
    }

    error = this->read_addresses(simulation_file);

    if (error)
    {
        std::cerr << "Error reading addresses." << std::endl;
        return error;
    }

    if (this->flags.file_verbose)
    {
        for (auto entry : this->processes)
        {
            std::cout << "Process " << entry.first << ": Size: " << entry.second->size() << std::endl;
        }

        for (auto entry : this->virtual_addresses)
        {
            std::cout << entry << std::endl;
        }
    }

    return 0;
}
