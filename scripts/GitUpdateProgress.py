from git import UpdateProgress

class GitUpdateProgress(UpdateProgress):
    def update(self, op_code: int, cur_count: str | float, max_count: str | float | None = None, message: str = "") -> None:
        print(op_code)
        print(cur_count)
        print(max_count)
        print(message)
        return super().update(op_code, cur_count, max_count, message)