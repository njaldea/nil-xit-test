#include <nil/xit/test.hpp>

// TODO: use better runner like asio/parallel
#include <nil/gate/runners/NonBlocking.hpp>

namespace nil::xit::test
{
    App::App(nil::service::S& service, std::string_view app_name)
        : xit(nil::xit::make_core(service))
    {
        gate.set_runner<nil::gate::runners::NonBlocking>();
        on_ready(service, [this]() { gate.commit(); });
        set_cache_directory(xit, std::filesystem::temp_directory_path() / app_name);
    }

    App::App(nil::service::HTTPService& service, std::string_view app_name)
        : xit(nil::xit::make_core(service))
    {
        gate.set_runner<nil::gate::runners::NonBlocking>();
        on_ready(service, [this]() { gate.commit(); });
        set_cache_directory(xit, std::filesystem::temp_directory_path() / app_name);
    }

    const std::vector<std::string>& App::installed_tags() const
    {
        return tags;
    }
}
