# CLI Options

## Headless Mode

| name    | short name | description                             |
|---------|------------|-----------------------------------------|
| help    | h          | show this help                          |
| list    | l          | list available tests                    |
| verbose | v          | list additional information like groups |
| ignore-missing-groups | i | ignore missing groups              |
| path-group | g       | add a group path mapping (see [Groups](./03-concepts.md#group)) |

```bash
./<binary> -hlvi -g "key=value"
```

## GUI Mode

| name    | short name | description                             |
|---------|------------|-----------------------------------------|
| help    | h          | show this help                          |
| list    | l          | list available tests                    |
| verbose | v          | list additional information like groups |
| clear   | c          | clear cache                             |
| ignore-missing-groups | i | ignore missing groups              |
| host    |            | use as host for the server              |
| port    | p          | use as port for the server              |
| path-group  | g      | add a group path mapping (see [Groups](./03-concepts.md#group)) |
| path-assets | a      | use for assets                          |

```bash
./<binary> gui -hlvci -g "key=value" -p 1101 -a "path"
```

## Group Path Mapping

The `-g` or `--path-group` option maps a group identifier to a filesystem path:

```
-g "group=path/to/directory"
```

For example:
```
-g "test=./test_resources" -g "base=./ui_components"
```

This allows tests and frames to reference resources using the `$group/path` syntax. For example, `$test/data.json` would resolve to `./test_resources/data.json` with the mapping above.

The `-i` or `--ignore-missing-groups` option prevents errors when encountering undefined groups. Without this option, the application will fail if a test references a group that isn't defined with `-g`.

See the [Groups concept documentation](./03-concepts.md#group) for more information on how group paths work.
