local c_group = vim.api.nvim_create_augroup("BitTwiddlingCFiles", { clear = true })

vim.api.nvim_create_autocmd("FileType", {
    desc = "C file settings",
    group = c_group,
    pattern = { "c", "cpp" }, -- cpp is just for the headers
    callback = function()
        vim.opt_local.makeprg = ".\\build.exe"
    end,
})
