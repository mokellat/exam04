#include "microshell.h"
#include <sys/wait.h>

int ft_strlen(char *s)
{
    int i = 0;
    while(s[i])
        i++;
    return(i);
}

int main(int argc, char **argv, char **env)
{
    int i = 1;
    int fd[2];
    int pid[500];
    int pp=0;
    int in = 0;
    int out = 1;
    int p_in = 0;
    if (argc > 1)
    {
        while(i < argc)
        {
            char *args[500];
            int cc = 0;
            while(i < argc && !strcmp(argv[i], "|") && !strcmp(argv[i],";"))
            {
                args[cc] = argv[i];
                cc++;
                i++;
                args[cc]=0; // why??
            }
            if(cc == 0)
            {
                i++;
                continue;
            }
            if(i < argc && !strcmp(argv[i], "|"))
            {
                if(pipe(fd) < 0)
                {
                    write(2, "error : fatal\n", 14);
					exit(0);
                }
                i++;
            }
            if(!strcmp(argv[i], "cd"))
            {
                if (cc != 2)
                    write(2, "error: cd: bad arguments", 25);
                else if (chdir(args[1]) == -1)
                    write(2, "error: cd: cannot change directory to ", 39), write(2, args[1], ft_strlen(args[1])), write(2, "\n", 1);
            }
            else
            {
                pid[pp]=fork();
                if(pid[pp] < 0)
                {
                    write(2, "error : fatal\n", 14);
					exit(0);
                }
                if(pid[pp] == 0)
                {
                    dup2(0,0);
                    dup2(1,1);
                    if (p_in != 0)
                        close(p_in);
                    if (in != 0)
                        close(in);
					if (out != 1)
						close(out);
                    execve(args[0], args, env);
                    //shouldnt we wait till it failed?
                    write(2, "error : couldn't execute ", 25), write(2, args[0], ft_strlen(args[0])), write(2, "\n", 1);
					exit(0);
                }
                else
                {
                    if (out != 1)
						close(out);
					if (in != 0)
						close(in);
					in = p_in;
					pp++;
                }
                if(i >= argc || (i < argc && !strcmp(argv[i], ";")))
                {
                    while(pp)
                    {
                        waitpid(pid[pp], 0, 0);
						pp--;
                    }
                    waitpid(pid[pp], 0, 0);
                    in = 0;
					p_in = 0;
					out = 1;
					i++;
                }
            }
        }
    }
}