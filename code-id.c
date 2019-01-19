#include <git2/errors.h>
#include <git2/global.h>
#include <git2/oid.h>
#include <git2/refs.h>
#include <git2/repository.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        char *prog = argc ? argv[0] : "code-id";
        fprintf(stderr, "Usage: %s <environment>\n", prog);
        return EXIT_FAILURE;
    }

    int status = EXIT_FAILURE;

    if (git_libgit2_init() < 0) {
        fprintf(stderr, "failed to init libgit2: %s\n", giterr_last()->message);
        goto libgit2_init_failed;
    }

    char *env = argv[1];
    char *repo_path;
    if (asprintf(&repo_path, "/etc/puppetlabs/code/environments/%s", env) < 0) {
        fputs("failed to create repository path\n", stderr);
        goto repo_path_failed;
    }

    git_repository *repo;
    if (git_repository_open(&repo, repo_path) < 0) {
        fprintf(stderr, "failed to open repository: %s\n", giterr_last()->message);
        goto repo_open_failed;
    }

    git_reference *reference;
    if (git_repository_head(&reference, repo) < 0) {
        fprintf(stderr, "failed to get repository head: %s\n", giterr_last()->message);
        goto repo_head_failed;
    }

    const git_oid *oid = git_reference_target(reference);
    printf("%s\n", git_oid_tostr_s(oid));
    status = EXIT_SUCCESS;

    git_reference_free(reference);
repo_head_failed:
    git_repository_free(repo);
repo_open_failed:
    free(repo_path);
repo_path_failed:
    git_libgit2_shutdown();
libgit2_init_failed:
    return status;
}
