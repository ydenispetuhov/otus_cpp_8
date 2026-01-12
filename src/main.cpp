#include "mask_filesystem_traverser.h"
#include "duplicate_finder.h"
#include "hash/crc32.h"
#include "hash/crc16.h"
#include <cstdlib>

#include <boost/program_options.hpp>

#include <iostream>
#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <stdexcept>

#include "filesystem_travercer.h"

namespace fs = std::filesystem;

template <typename Hash>
void job(std::size_t block_size, my::IFilesystemTraverser& filesystem_traverser)
{
    my::DuplicateFinder<Hash> duplicate_finder(block_size);
    duplicate_finder.add_filesystem_traverser(filesystem_traverser);

    std::vector<std::vector<fs::path>> equal = duplicate_finder.get_duplicates();
    for (const std::vector<fs::path>& group : equal)
    {
        for (const fs::path& path : group)
            std::cout << path << '\n';
        std::cout << '\n';
    }
}

int main(int argc, char* argv[]) try {
    // Parse command line options
    my::ProgramOptions options = my::parse_command_line(argc, argv);
    
    // If help was requested, we've already printed it and exited
    if (options.show_help) {
        return 0;
    }
    
    // Validate options
    my::validate_options(options);
    
    // Create filesystem traverser
    auto filesystem_traverser = my::create_filesystem_traverser(options);
    
    // Run duplicate finding based on hash algorithm
    if (options.hash_algorithm == "crc32") {
        my::find_and_print_duplicates<my::Crc32>(options.block_size, filesystem_traverser.get());
    } else if (options.hash_algorithm == "crc16") {
        my::find_and_print_duplicates<my::Crc16>(options.block_size, filesystem_traverser.get());
    } else {
        throw std::invalid_argument("Incorrect hash algorithm: " + options.hash_algorithm + 
                                   ". Supported algorithms are 'crc32' and 'crc16'.");
    }
    
    return 0;
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
} catch (...) {
    std::cerr << "Unknown error occurred\n";
    return 1;
}

namespace my {
    ProgramOptions parse_command_line(int argc, char* argv[]) {
        po::options_description desc("Allowed options");
        ProgramOptions options;
        
        // Define command line options
        desc.add_options()
            ("help", "Print this help message")
            ("size", po::value<std::size_t>(&options.block_size)->default_value(4096),
             "Block size (in bytes) used to compare files (minimum 1)")
            ("hash", po::value<std::string>(&options.hash_algorithm)->default_value("crc32"),
             "Hash algorithm used to compare byte blocks ('crc32' or 'crc16')")
            ("min_file_size", po::value<std::size_t>(&options.min_file_size)->default_value(1),
             "Minimum file size to compare (in bytes)")
            ("root_dir", po::value<std::vector<std::string>>(&options.root_directories)->required()->multitoken(),
             "Directories to search for duplicates")
            ("exclude_dir", po::value<std::vector<std::string>>(&options.exclude_directories)->multitoken(),
             "Directories to exclude from search")
            ("mask_include", po::value<std::vector<std::string>>(&options.masks_include)->multitoken(),
             "Include only files matching these regex patterns")
            ("mask_exclude", po::value<std::vector<std::string>>(&options.masks_exclude)->multitoken(),
             "Exclude files matching these regex patterns")
            ("recursive", po::bool_switch(&options.recursive),
             "Enable recursive subdirectory scanning")
            ("case_sensitive", po::bool_switch(&options.case_sensitive),
             "Make file masks case sensitive")
        ;
        
        po::variables_map var_map;
        try {
            po::store(po::parse_command_line(argc, argv, desc), var_map);
            po::notify(var_map);
        } catch (const po::error& error) {
            throw std::runtime_error("Error parsing command-line arguments: " + std::string(error.what()) + 
                                   "\nUse --help for usage information.");
        }
        
        // Check if help was requested
        if (var_map.count("help")) {
            print_help(desc);
            options.show_help = true;
        }
        
        return options;
    }
    catch (const po::error& error)
    {
        std::cerr << "Error while parsing command-line arguments: "
                  << error.what() << "\nPlease use --help to see help message\n";
        std::exit(EXIT_FAILURE);
    }

    std::size_t block_size = var_map["size"].as<std::size_t>();
    if (block_size == 0)
    {
        std::cerr << "Block size must be at least 1\n";
        std::exit(EXIT_FAILURE);
    }
    std::size_t min_file_size = var_map["min_file_size"].as<std::size_t>();

    std::vector<std::string> root_directories    = var_map["root_dir"].as<std::vector<std::string>>();

    std::vector<std::string> exclude_directories = var_map.count("exclude_dir") == 0
                                                   ? std::vector<std::string>()
                                                   : var_map["exclude_dir"].as<std::vector<std::string>>();

    std::vector<std::string> masks_include       = var_map.count("mask_include") == 0
                                                   ? std::vector<std::string>()
                                                   : var_map["mask_include"].as<std::vector<std::string>>();

    std::vector<std::string> masks_exclude       = var_map.count("mask_exclude") == 0
                                                   ? std::vector<std::string>()
                                                   : var_map["mask_exclude"].as<std::vector<std::string>>();

    my::MaskFilesystemTraverser filesystem_traverser(recursive);

    for (std::string& root_directory : root_directories)
        filesystem_traverser.add_root_directory(fs::path(std::move(root_directory)));
    for (std::string& exclude_directory : exclude_directories)
        filesystem_traverser.add_exclude_directory(fs::path(std::move(exclude_directory)));

    std::regex::flag_type re_flags = (case_sensitive ? std::regex_constants::ECMAScript : std::regex_constants::icase);
    for (const std::string& mask_include : masks_include)
        filesystem_traverser.add_file_mask_include(std::regex(mask_include, re_flags));
    for (const std::string& mask_exclude : masks_exclude)
        filesystem_traverser.add_file_mask_exclude(std::regex(mask_exclude, re_flags));

    filesystem_traverser.set_min_file_size(min_file_size);

    std::string hash_algo = var_map["hash"].as<std::string>();

    if (hash_algo == "crc32")
        job<my::Crc32>(block_size, filesystem_traverser);
    else if (hash_algo == "crc16")
        job<my::Crc16>(block_size, filesystem_traverser);
    else
        throw std::invalid_argument(std::string("Incorrect hash algorithm ") + hash_algo + ". Use --help options for more details");

    std::exit(EXIT_SUCCESS);
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
    std::exit(EXIT_FAILURE);
}