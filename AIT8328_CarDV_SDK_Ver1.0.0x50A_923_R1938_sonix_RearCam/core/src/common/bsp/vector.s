;------------------------------------------------------------------------------
;- Area Definition
;-----------------
;- Must be defined as function to put first in the code as it must be mapped
;- at offset 0 of the flash EBI_CSR0, ie. at address 0 before remap.
;------------------------------------------------------------------------------
                AREA        vector, CODE, READONLY

    IMPORT InitReset
    IMPORT ARM_ExceptionHandler

;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
;- Exception vectors ( before Remap )
;------------------------------------
;- These vectors are read at address 0.
;- They absolutely requires to be in relative addresssing mode in order to
;- guarantee a valid jump. For the moment, all are just looping (what may be
;- dangerous in a final system). If an exception occurs before remap, this
;- would result in an infinite loop.
;------------------------------------------------------------------------------
                B           InitReset                   ; reset
undefvec
                B           ExceptUndefInstrHndlr       ; Undefined Instruction
swivec
                B           swivec                      ; Software Interrupt
pabtvec
                B           ExceptPrefetchAbortHndlr    ; Prefetch Abort
dabtvec
                B           ExceptDataAbortHndlr        ; Data Abort
rsvdvec
                B           rsvdvec                     ; reserved
irqvec
                ldr         pc, [pc,#-0xF20]            ; IRQ : read the AIC
fiqvec
                B           fiqvec                      ; FIQ

;-----------------------
;- The exception handler
;-----------------------
ExceptUndefInstrHndlr
    sub     lr, lr, #4
    mov     r1, lr
    mov     r0, #4
	B		ExceptHndlr
ExceptPrefetchAbortHndlr
    sub     lr, lr, #4
    mov     r1, lr
    mov     r0, #12
	B		ExceptHndlr
ExceptDataAbortHndlr
    sub     lr, lr, #8
    mov     r1, lr
    mov     r0, #16
	B		ExceptHndlr
ExceptHndlr
    B       ARM_ExceptionHandler

            END