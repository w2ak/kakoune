#include "shell_manager.hh"

#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>

namespace Kakoune
{

ShellManager::ShellManager()
   : m_regex(LR"(\$\{kak_([a-z0-9_]+)[^}]*\}|\$kak_([a-z0-9_]+))")
{
}

String ShellManager::eval(const String& cmdline, const Context& context)
{
    int write_pipe[2];
    int read_pipe[2];

    pipe(write_pipe);
    pipe(read_pipe);

    String output;
    if (pid_t pid = fork())
    {
        close(write_pipe[0]);
        close(read_pipe[1]);
        close(write_pipe[1]);

        char buffer[1024];
        while (size_t size = read(read_pipe[0], buffer, 1024))
        {
            if (size == -1)
                break;
            output += String(buffer, buffer+size);
        }
        close(read_pipe[0]);
        waitpid(pid, NULL, 0);
    }
    else
    {
        close(write_pipe[1]);
        close(read_pipe[0]);

        dup2(read_pipe[1], 1);
        dup2(write_pipe[0], 0);

        boost::regex_iterator<String::iterator> it(cmdline.begin(), cmdline.end(), m_regex);
        boost::regex_iterator<String::iterator> end;

        while (it != end)
        {
            auto& match = *it;

            String name;
            if (match[1].matched)
                name = String(match[1].first, match[1].second);
            else if (match[2].matched)
                name = String(match[2].first, match[2].second);
            else
                assert(false);
            assert(name.length() > 0);

            auto env_var = m_env_vars.find(name);
            if (env_var != m_env_vars.end())
            {
                String value = env_var->second(context);
                setenv(("kak_" + name).c_str(), value.c_str(), 1);
            }

            ++it;
        }

        execlp("sh", "sh", "-c", cmdline.c_str(), NULL);
    }
    return output;
}

void ShellManager::register_env_var(const String& name,
                                    EnvVarRetriever retriever)
{
    m_env_vars[name] = std::move(retriever);
}

}
