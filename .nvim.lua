local c_group = vim.api.nvim_create_augroup("BitTwiddlingCFiles", { clear = true })

vim.api.nvim_create_autocmd("FileType", {
    desc = "C file settings",
    group = c_group,
    pattern = { "c", "cpp" }, -- cpp is just for the headers
    callback = function()
        vim.opt_local.wrap = false
        vim.opt_local.list = true
        vim.opt_local.listchars = "extends:»"
        vim.opt_local.makeprg = ".\\build.exe"
    end,
})

vim.api.nvim_create_autocmd("BufNewFile", {
    desc = "Insert boilerplate into new C files",
    group = c_group,
    pattern = "*.c",
    callback = function()
        vim.api.nvim_buf_set_lines(0, 0, 0, false, { "#include <stdint.h>", "", "int main(void)", "{", "    ", "    return 0;", "}" })
        vim.api.nvim_win_set_cursor(0, { 5, 5 })
        vim.cmd.startinsert()
    end,
})

vim.lsp.enable('typst_ls')
vim.lsp.enable('harper_ls')
