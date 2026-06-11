space global:

    var x = 1

    class A:
        var x

        fun get():
            ret x


    fun main():
        
        var a = new A(69)

        print(a.get())

        ret 0
