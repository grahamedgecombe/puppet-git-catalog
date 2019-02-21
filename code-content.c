#include <git2/blob.h>
#include <git2/commit.h>
#include <git2/errors.h>
#include <git2/global.h>
#include <git2/repository.h>
#include <git2/submodule.h>
#include <git2/tree.h>
#include <git2/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        char *prog = argc ? argv[0] : "code-content";
        fprintf(stderr, "Usage: %s <environment> <code-id> <file-path>\n", prog);
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

    char *code_id = argv[2];
    git_oid commit_oid;
    if (git_oid_fromstr(&commit_oid, code_id) < 0) {
        fprintf(stderr, "invalid code-id: %s\n", giterr_last()->message);
        goto commit_lookup_failed;
    }

    git_commit *commit;
    if (git_commit_lookup(&commit, repo, &commit_oid) < 0) {
        fprintf(stderr, "failed to lookup commit: %s\n", giterr_last()->message);
        goto commit_lookup_failed;
    }

    git_tree *tree;
    if (git_commit_tree(&tree, commit) < 0) {
        fprintf(stderr, "failed to lookup tree: %s\n", giterr_last()->message);
        goto tree_lookup_failed;
    }

    char *path = argv[3];
    char *submodule_path = path;
    for (;;) {
        char *path_next = 0;
        char *path_sep = strchr(path, '/');
        if (path_sep) {
            path_next = path_sep + 1;
            *path_sep = 0;
        }

        const git_tree_entry *entry = git_tree_entry_byname(tree, path);
        if (!entry) {
            fputs("entry does not exist\n", stderr);
            goto tree_entry_failed;
        }

        const git_oid *entry_oid = git_tree_entry_id(entry);
        git_otype entry_type = git_tree_entry_type(entry);

        if (entry_type == GIT_OBJ_TREE) {
            if (!path_next) {
                fputs("entry is not a file\n", stderr);
                goto tree_entry_failed;
            }

            git_tree *next_tree;
            if (git_tree_lookup(&next_tree, repo, entry_oid) < 0) {
                fprintf(stderr, "failed to lookup tree: %s\n", giterr_last()->message);
                goto tree_entry_failed;
            }

            git_tree_free(tree);
            tree = next_tree;
        } else if (entry_type == GIT_OBJ_BLOB) {
            if (path_next) {
                fputs("entry is not a directory\n", stderr);
                goto tree_entry_failed;
            }

            git_blob *blob;
            if (git_blob_lookup(&blob, repo, entry_oid) < 0) {
                fprintf(stderr, "failed to lookup blob: %s\n", giterr_last()->message);
                goto tree_entry_failed;
            }

            if (fwrite(git_blob_rawcontent(blob), (size_t) git_blob_rawsize(blob), 1, stdout) != 1) {
                fputs("failed to write blob\n", stderr);
                goto fwrite_failed;
            }

            status = EXIT_SUCCESS;

fwrite_failed:
            git_blob_free(blob);
            break;
        } else if (entry_type == GIT_OBJ_COMMIT) {
            if (!path_next) {
                fputs("entry is not a file\n", stderr);
                goto tree_entry_failed;
            }

            bool success = false;

            git_submodule *submodule;
            if (git_submodule_lookup(&submodule, repo, submodule_path) < 0) {
                fprintf(stderr, "failed to lookup submodule: %s\n", giterr_last()->message);
                goto submodule_lookup_failed;
            }

            git_repository *next_repo;
            if (git_submodule_open(&next_repo, submodule) < 0) {
                fprintf(stderr, "failed to open submodule: %s\n", giterr_last()->message);
                goto submodule_open_failed;
            }

            git_commit *next_commit;
            if (git_commit_lookup(&next_commit, next_repo, entry_oid) < 0) {
                fprintf(stderr, "failed to lookup commit: %s\n", giterr_last()->message);
                goto submodule_commit_lookup_failed;
            }

            git_tree *next_tree;
            if (git_commit_tree(&next_tree, next_commit) < 0) {
                fprintf(stderr, "failed to lookup tree: %s\n", giterr_last()->message);
                goto submodule_tree_lookup_failed;
            }

            /* swap repo, commit and tree */
            git_repository *tmp_repo = repo;
            repo = next_repo;
            next_repo = tmp_repo;

            git_commit *tmp_commit = commit;
            commit = next_commit;
            next_commit = tmp_commit;

            git_tree *tmp_tree = tree;
            tree = next_tree;
            next_tree = tmp_tree;

            submodule_path = path_next;
            success = true;

            git_tree_free(next_tree);
submodule_tree_lookup_failed:
            git_commit_free(next_commit);
submodule_commit_lookup_failed:
            git_repository_free(next_repo);
submodule_open_failed:
            git_submodule_free(submodule);
submodule_lookup_failed:
            if (!success) {
                goto tree_entry_failed;
            }
        } else {
            fprintf(stderr, "unsupported object type: %s\n", git_object_type2string(entry_type));
            goto tree_entry_failed;
        }

        path = path_next;
        *path_sep = '/';
    }

tree_entry_failed:
    git_tree_free(tree);
tree_lookup_failed:
    git_commit_free(commit);
commit_lookup_failed:
    git_repository_free(repo);
repo_open_failed:
    free(repo_path);
repo_path_failed:
    git_libgit2_shutdown();
libgit2_init_failed:
    return status;
}
